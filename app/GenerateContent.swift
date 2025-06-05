import ArgumentParser
import Foundation
import GoogleGenerativeAI

@main
struct GenerateContent: AsyncParsableCommand {
    @Option(help: "The API key to use when calling the Generative Language API.")
    var apiKey: String

    @Option(name: .customLong("model"), help: "The name of the model to use (e.g., 'RODA').")
    var modelName: String?

    @Option(help: "The text prompt for the model in natural language.")
    var textPrompt: String?

    @Option(
        name: .customLong("image-path"),
        help: "The file path of an image to pass to the model; must be in JPEG or PNG format.",
        transform: URL.filePath(_:)
    )
    var imageURL: URL?

    @Option(
        name: .customLong("output-file"),
        help: "The file path where the generated content should be saved."
    )
    var outputFilePath: String?

    @Flag(
        name: .customLong("streaming"),
        help: "Stream response data, printing it incrementally as it's received."
    ) var isStreaming = false

    @Flag(
        name: .customLong("GoogleGenerativeAIDebugLogEnabled", withSingleDash: true),
        help: "Enable additional debug logging."
    ) var debugLogEnabled = false

    mutating func validate() throws {
        if textPrompt == nil && imageURL == nil {
            throw ValidationError(
                "Missing expected argument(s) '--text-prompt <text-prompt>' and/or" +
                " '--image-path <image-path>'."
            )
        }
    }

    mutating func run() async throws {
        do {
            let modelName = modelNameOrDefault()

            let model = try GenerativeModel(name: modelName, apiKey: apiKey)

            var parts = [ModelContent.Part]()

            if let textPrompt = textPrompt {
                parts.append(.text(textPrompt))
            }

            if let imageURL = imageURL {
                let mimeType: String
                switch imageURL.pathExtension.lowercased() {
                case "jpg", "jpeg":
                    mimeType = "image/jpeg"
                case "png":
                    mimeType = "image/png"
                default:
                    throw CLIError.unsupportedImageType
                }
                let imageData = try Data(contentsOf: imageURL)
                parts.append(.data(mimetype: mimeType, imageData))
            }

            let input = [ModelContent(parts: parts)]

            if isStreaming {
                let contentStream = try await model.generateContentStream(input)
                print("Generated Content (streaming):")
                for try await content in contentStream {
                    if let text = content.text {
                        print(text)
                        // Save content to file if outputFilePath is provided
                        if let outputFilePath = outputFilePath {
                            try saveToFile(content: text, filePath: outputFilePath)
                        }
                    }
                }
            } else {
                let content = try await model.generateContent(input)
                if let text = content.text {
                    print("Generated Content:\n\(text)")
                    // Save content to file if outputFilePath is provided
                    if let outputFilePath = outputFilePath {
                        try saveToFile(content: text, filePath: outputFilePath)
                    }
                }
            }
        } catch {
            print("Error during content generation: \(error)")
        }
    }

    // Helper function to save the content to a file
    func saveToFile(content: String, filePath: String) throws {
        let url = URL(fileURLWithPath: filePath)
        try content.write(to: url, atomically: true, encoding: .utf8)
        print("Content saved to \(filePath)")
    }

    func modelNameOrDefault() -> String {
        if let modelName {
            return modelName
        } else {
            return "RODA"
        }
    }
}

enum CLIError: Error {
    case unsupportedImageType
}

private extension URL {
    static func filePath(_ filePath: String) throws -> URL {
        return URL(fileURLWithPath: filePath)
    }
}

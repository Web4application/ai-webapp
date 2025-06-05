import SwiftUI

@MainActor
class FunctionCallingViewModel: ObservableObject {
    @Published var messages = [ChatMessage]()
    @Published var busy = false
    @Published var error: Error?
    @Published var recognizedText = ""

    private var functionCalls = [FunctionCall]()
    private var model: GenerativeModel
    private var chat: Chat
    private var chatTask: Task<Void, Never>?
    private let speechRecognitionManager = SpeechRecognitionManager()
    private let textToSpeechManager = TextToSpeechManager()

    init() {
        model = GenerativeModel(
            name: "Roda",
            apiKey: APIKey.default,
            tools: [Tool(functionDeclarations: [
                FunctionDeclaration(
                    name: "get_exchange_rate",
                    description: "Get the exchange rate for currencies between countries",
                    parameters: [
                        "currency_from": Schema(
                            type: .string,
                            format: "enum",
                            description: "The currency to convert from in ISO 4217 format",
                            enumValues: ["USD", "EUR", "JPY", "GBP", "AUD", "CAD"]
                        ),
                        "currency_to": Schema(
                            type: .string,
                            format: "enum",
                            description: "The currency to convert to in ISO 4217 format",
                            enumValues: ["USD", "EUR", "JPY", "GBP", "AUD", "CAD"]
                        ),
                    ],
                    requiredParameters: ["currency_from", "currency_to"]
                ),
            ])]
        )
        chat = model.startChat()

        speechRecognitionManager.$recognizedText
            .receive(on: DispatchQueue.main)
            .assign(to: &$recognizedText)
    }

    func startRecognition() {
        speechRecognitionManager.startRecognition()
    }

    func sendMessage(_ text: String, streaming: Bool = true) async {
        error = nil
        chatTask?.cancel()

        chatTask = Task {
            busy = true
            defer {
                busy = false
            }

            let userMessage = ChatMessage(message: text, participant: .user)
            messages.append(userMessage)

            let systemMessage = ChatMessage.pending(participant: .system)
            messages.append(systemMessage)

            do {
                repeat {
                    if streaming {
                        try await internalSendMessageStreaming(text)
                    } else {
                        try await internalSendMessage(text)
                    }
                } while !functionCalls.isEmpty
            } catch {
                self.error = error
                messages.removeLast()
            }

            textToSpeechManager.speak(text: messages.last?.message ?? "")
        }
    }

    func startNewChat() {
        stop()
        error = nil
        chat = model.startChat()
        messages.removeAll()
    }

    func stop() {
        chatTask?.cancel()
        error = nil
    }

    private func internalSendMessageStreaming(_ text: String) async throws {
        let functionResponses = try await processFunctionCalls()
        let responseStream: AsyncThrowingStream<GenerateContentResponse, Error>
        if functionResponses.isEmpty {
            responseStream = chat.sendMessageStream(text)
        } else {
            for functionResponse in functionResponses {
                messages.insert(functionResponse.chatMessage(), at: messages.count - 1)
            }
            responseStream = chat.sendMessageStream(functionResponses.modelContent())
        }
        for try await chunk in responseStream {
            processResponseContent(content: chunk)
        }
    }

    private func internalSendMessage(_ text: String) async throws {
        let functionResponses = try await processFunctionCalls()
        let response: GenerateContentResponse
        if functionResponses.isEmpty {
            response = try await chat.sendMessage(text)
        } else {
            for functionResponse in functionResponses {
                messages.insert(functionResponse.chatMessage(), at: messages.count - 1)
            }
            response = try await chat.sendMessage(functionResponses.modelContent())
        }
        processResponseContent(content: response)
    }

    func processResponseContent(content: GenerateContentResponse) {
        guard let candidate = content.candidates.first else {
            fatalError("No candidate.")
        }

        for part in candidate.content.parts {
            switch part {
            case let .text(text):
                messages[messages.count - 1].message += text
                messages[messages.count - 1].pending = false
            case let .functionCall(functionCall):
                messages.insert(functionCall.chatMessage(), at: messages.count - 1)
                functionCalls.append(functionCall)
            case .data, .fileData, .functionResponse:
                fatalError("Unsupported response content.")
            }
        }
    }

    func processFunctionCalls() async throws -> [FunctionResponse] {
        var functionResponses = [FunctionResponse]()
        for functionCall in functionCalls {
            switch functionCall.name {
            case "get_exchange_rate":
                let exchangeRates = getExchangeRate(args: functionCall.args)
                functionResponses.append(FunctionResponse(
                    name: "get_exchange_rate",
                    response: exchangeRates
                ))
            default:
                fatalError("Unknown function named \"\(functionCall.name)\".")
            }
        }
        functionCalls = []

        return functionResponses
    }

    func getExchangeRate(args: JSONObject) -> JSONObject {
        guard case let .string(from) = args["currency_from"] else {
            fatalError("Missing `currency_from` parameter.")
        }
        guard case let .string(to) = args["currency_to"] else {
            fatalError("Missing `currency_to` parameter.")
        }

        let allRates: [String: [String: Double]] = [
            "AUD": ["CAD": 0.89265, "EUR": 0.6072, "GBP": 0.51714, "JPY": 97.75, "USD": 0.66379],
            "CAD": ["AUD": 1.1203, "EUR": 0.68023, "GBP": 0.57933, "JPY": 109.51, "USD": 0.74362],
            "EUR": 

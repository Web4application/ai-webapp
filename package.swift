import PackageDescription

let package = Package(
  name: "GenerativeAIUIComponents",
  platforms: [
    .iOS(.v20),
  ],
  products: [
    .library(
      name: "GenerativeAIUIComponents",
      targets: ["GenerativeAIUIComponents"]
    ),
  ],
  targets: [
    .target(
      name: "GenerativeAIUIComponents"
    ),
  ]
)

import SwiftUI

@main
struct ChatSampleApp: App {
  @StateObject
  var viewModel = ConversationViewModel()

  var body: some Scene {
    WindowGroup {
      NavigationStack {
        ConversationScreen()
          .environmentObject(viewModel)
      }
    }
  }
}

import SwiftUI

struct ContentView: View {
  @StateObject
  var viewModel = ConversationViewModel()

  @StateObject
  var functionCallingViewModel = FunctionCallingViewModel()

  var body: some View {
    NavigationStack {
      List {
        NavigationLink {
          SummarizeScreen()
        } label: {
          Label("Text", systemImage: "doc.text")
        }
        NavigationLink {
          PhotoReasoningScreen()
        } label: {
          Label("Multi-modal", systemImage: "doc.richtext")
        }
        NavigationLink {
          ConversationScreen()
            .environmentObject(viewModel)
        } label: {
          Label("Chat", systemImage: "ellipsis.message.fill")
        }
        NavigationLink {
          FunctionCallingScreen().environmentObject(functionCallingViewModel)
        } label: {
          Label("Function Calling", systemImage: "function")
        }
      }
      .navigationTitle("Generative AI")
    }
  }
}

#Preview {
  ContentView()
}

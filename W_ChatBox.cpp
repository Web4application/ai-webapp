// Copyright Discord, Inc. All Rights Reserved.

#include "W_ChatBox.h"

#include "DiscordLocalPlayerSubsystem.h"

#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

#include "W_FrontEndBase.h"

void UW_ChatBox::NativeOnInitialized()
{
    SendChatMessage->OnClicked.AddDynamic(this, &UW_ChatBox::OnClickedSendChatMessage);
    SendMessageCallback.BindDynamic(this, &UW_ChatBox::OnMessageSendFinished);
    ChatTargetComboBox->OnSelectionChanged.AddDynamic(this, &UW_ChatBox::OnChangedChatTarget);
}

void UW_ChatBox::BindClientEvents()
{
    if (!Client) {
        return;
    }

    auto Subsystem = GetOwningLocalPlayer()->GetSubsystem<UDiscordLocalPlayerSubsystem>();

    Subsystem->OnMessageCreated.RemoveAll(this);
    Subsystem->OnMessageUpdated.RemoveAll(this);
    Subsystem->OnMessageDeleted.RemoveAll(this);
    Subsystem->OnMessageCreated.AddDynamic(this, &UW_ChatBox::OnMessageCreated);
    Subsystem->OnMessageUpdated.AddDynamic(this, &UW_ChatBox::OnMessageUpdated);
    Subsystem->OnMessageDeleted.AddDynamic(this, &UW_ChatBox::OnMessageDeleted);
    RefreshChatTargetList();
}

void UW_ChatBox::OnUserContextChanged(UW_DiscordUserInfo* UserInfo)
{
    CurrentUserInfo = UserInfo;
    RefreshChatTargetList();
}

void UW_ChatBox::OnChangedChatTarget(FString Item, ESelectInfo::Type Info)
{
    auto SelectedIndex = ChatTargetComboBox->GetSelectedIndex();

    if (SelectedIndex == 0 && !CurrentUserInfo.IsValid()) {
        auto Title = FString(UTF8_TO_TCHAR("Chat"));
        ChatTitleText->SetText(FText::FromString(Title));

        auto Hint = FString(UTF8_TO_TCHAR("Select a friend to message"));
        ChatMessageText->SetHintText(FText::FromString(Hint));

        SendChatMessage->SetIsEnabled(false);
    }
    else {
        auto Title = FString(UTF8_TO_TCHAR("Chat to ")) + ChatTargetComboBox->GetSelectedOption();
        ChatTitleText->SetText(FText::FromString(Title));

        auto Hint = FString(UTF8_TO_TCHAR("Message ")) + ChatTargetComboBox->GetSelectedOption();
        ChatMessageText->SetHintText(FText::FromString(Hint));

        SendChatMessage->SetIsEnabled(true);
    }
}

void UW_ChatBox::OnClickedSendChatMessage()
{
    if (ChatTargetComboBox->GetSelectedIndex() == 0) {
        if (!CurrentUserInfo.IsValid()) {
            return;
        }

        if (!Client) {
            return;
        }

        auto MessageText = ChatMessageText->GetText().ToString();
        Client->SendUserMessage(CurrentUserInfo->User->Id(), MessageText, SendMessageCallback);
    }
    else {
        FDiscordUniqueID RecipientID{};

        auto SelectedLobby = ChatTargetComboBox->GetSelectedOption();
        SelectedLobby = SelectedLobby.Mid(6, SelectedLobby.Len() - 6);

        auto AllLobbies = Client->GetLobbyIds();
        for (auto idx = 0; idx < AllLobbies.Num(); ++idx) {
            if (AllLobbies[idx].ToString() == SelectedLobby) {
                RecipientID = AllLobbies[idx];
                break;
            }
        }

        if (RecipientID != 0) {
            auto MessageText = ChatMessageText->GetText().ToString();
            Client->SendLobbyMessage(RecipientID, MessageText, SendMessageCallback);
        }
    }

    LoadingSwitcher->SetActiveWidgetIndex(1);
}

void UW_ChatBox::OnMessageSendFinished(UDiscordClientResult* Result, FDiscordUniqueID MessageID)
{
    if (Result->Successful()) {
        ChatMessageText->SetText(FText::GetEmpty());
    }

    LoadingSwitcher->SetActiveWidgetIndex(0);
}

void UW_ChatBox::OnMessageCreated(FDiscordUniqueID MessageID)
{
    if (!Client) {
        return;
    }

    auto messageHandle = Client->GetMessageHandle(MessageID);

    auto channelHandle = Client->GetChannelHandle(messageHandle->ChannelId());
    auto lobbyHandle = Client->GetLobbyHandle(messageHandle->ChannelId());

    if ((channelHandle && channelHandle->Type() == EDiscordChannelType::Dm) || lobbyHandle) {
        UW_DiscordMessageWrapper* wrapper = NewObject<UW_DiscordMessageWrapper>();
        wrapper->Client = Client;
        wrapper->Message = messageHandle;
        wrapper->Lobby = lobbyHandle;
        ChatMessageList->AddItem(wrapper);

        ChatMessageList->ScrollIndexIntoView(ChatMessageList->GetNumItems() - 1);
    }
}

void UW_ChatBox::OnMessageUpdated(FDiscordUniqueID MessageID)
{
}

void UW_ChatBox::OnMessageDeleted(FDiscordUniqueID MessageID, FDiscordUniqueID ChannelID)
{
    UW_DiscordMessageWrapper* toBeRemoved = nullptr;
    for (int32 idx = 0; idx < ChatMessageList->GetNumItems(); ++idx) {
        auto* listItem = ChatMessageList->GetItemAt(idx);
        auto* messageWrapper = Cast<UW_DiscordMessageWrapper>(listItem);
        if (messageWrapper && messageWrapper->Message->Id() == MessageID) {
            toBeRemoved = messageWrapper;
            break;
        }
    }

    if (toBeRemoved != nullptr) {
        ChatMessageList->RemoveItem(toBeRemoved);
    }
}

void UW_ChatBox::OnJoinLobby(FDiscordUniqueID LobbyID)
{
    if (!Client) {
        return;
    }

    UW_DiscordMessageWrapper* wrapper = NewObject<UW_DiscordMessageWrapper>();
    wrapper->MessageContent.Append("[")
      .Append(LobbyID.ToString())
      .Append("] ")
      .Append("You joined. (It was created?)");
    ChatMessageList->AddItem(wrapper);

    RefreshChatTargetList();
}

void UW_ChatBox::OnLeaveLobby(FDiscordUniqueID LobbyID)
{
    if (!Client) {
        return;
    }

    UW_DiscordMessageWrapper* wrapper = NewObject<UW_DiscordMessageWrapper>();
    wrapper->MessageContent.Append("[")
      .Append(LobbyID.ToString())
      .Append("] ")
      .Append("You left. (It was destroyed?)");
    ChatMessageList->AddItem(wrapper);

    RefreshChatTargetList();
}

void UW_ChatBox::OnLobbyMemberAdd(FDiscordUniqueID LobbyID, FDiscordUniqueID MemberID)
{
    if (!Client) {
        return;
    }

    auto UserHandle = Client->GetUser(MemberID);
    auto RawUserDisplayName = UserHandle->DisplayName();
    auto UserDisplayName =
      (RawUserDisplayName.Len() > 0) ? RawUserDisplayName : TEXT("UnknownUser");

    FString MessageContent;
    MessageContent.Append("[")
      .Append(LobbyID.ToString())
      .Append("] ")
      .Append(UserDisplayName)
      .Append(" added to the lobby.");

    UW_DiscordMessageWrapper* wrapper = NewObject<UW_DiscordMessageWrapper>();
    wrapper->MessageContent = std::move(MessageContent);
    ChatMessageList->AddItem(wrapper);

    RefreshChatTargetList();
}

void UW_ChatBox::OnLobbyMemberUpdate(FDiscordUniqueID LobbyID, FDiscordUniqueID MemberID)
{
    if (!Client) {
        return;
    }

    auto UserHandle = Client->GetUser(MemberID);
    auto RawUserDisplayName = UserHandle->DisplayName();
    auto UserDisplayName =
      (RawUserDisplayName.Len() > 0) ? RawUserDisplayName : TEXT("UnknownUser");
    auto handle = Client->GetLobbyHandle(LobbyID)->GetLobbyMemberHandle(MemberID);
    bool connected = !!handle && handle->Connected();

    FString MessageContent;
    MessageContent.Append("[")
      .Append(LobbyID.ToString())
      .Append("] ")
      .Append(UserDisplayName)
      .Append(" updated and is ")
      .Append(connected ? "connected" : "disconnected");

    UW_DiscordMessageWrapper* wrapper = NewObject<UW_DiscordMessageWrapper>();
    wrapper->MessageContent = std::move(MessageContent);
    ChatMessageList->AddItem(wrapper);

    RefreshChatTargetList();
}

void UW_ChatBox::OnLobbyMemberRemove(FDiscordUniqueID LobbyID, FDiscordUniqueID MemberID)
{
    if (!Client) {
        return;
    }

    auto UserHandle = Client->GetUser(MemberID);
    auto RawUserDisplayName = UserHandle->DisplayName();
    auto UserDisplayName =
      (RawUserDisplayName.Len() > 0) ? RawUserDisplayName : TEXT("UnknownUser");

    FString MessageContent;
    MessageContent.Append("[")
      .Append(LobbyID.ToString())
      .Append("] ")
      .Append(UserDisplayName)
      .Append(" removed from the lobby.");

    UW_DiscordMessageWrapper* wrapper = NewObject<UW_DiscordMessageWrapper>();
    wrapper->MessageContent = std::move(MessageContent);
    ChatMessageList->AddItem(wrapper);

    RefreshChatTargetList();
}

void UW_ChatBox::RefreshChatTargetList()
{
    if (!Client) {
        return;
    }

    auto OldSelectedIndex = ChatTargetComboBox->GetSelectedIndex();
    auto OldSelectedOption = ChatTargetComboBox->GetSelectedOption();

    int IndexToSelect = 0;

    ChatTargetComboBox->ClearOptions();
    ChatTargetComboBox->ClearSelection();

    if (CurrentUserInfo.IsValid()) {
        ChatTargetComboBox->AddOption(CurrentUserInfo->User->DisplayName());
    }
    else {
        ChatTargetComboBox->AddOption(FString("Select User"));
    }

    auto AllLobbies = Client->GetLobbyIds();
    for (auto idx = 0; idx < AllLobbies.Num(); ++idx) {
        ChatTargetComboBox->AddOption(FString("Lobby ").Append(AllLobbies[idx].ToString()));
    }

    ChatTargetComboBox->SetSelectedIndex(IndexToSelect);
}

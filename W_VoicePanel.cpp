// Copyright Discord, Inc. All Rights Reserved.

#include "W_VoicePanel.h"

#if PLATFORM_ANDROID
#include "AndroidPermissionCallbackProxy.h"
#include "AndroidPermissionFunctionLibrary.h"
#endif

#include "DiscordLocalPlayerSubsystem.h"

#include "W_VoiceParticipantEntry.h"

#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"

#include "DiscordxUnreal.h"

void UW_VoicePanel::NativeOnInitialized()
{
    ConnectButton->OnClicked.AddDynamic(this, &UW_VoicePanel::OnClickedConnect);
    DisconnectButton->OnClicked.AddDynamic(this, &UW_VoicePanel::OnClickedDisconnect);
    SelfDeafButton->OnClicked.AddDynamic(this, &UW_VoicePanel::OnClickedSelfDeaf);

    ConnectButton->SetIsEnabled(false);
    DisconnectButton->SetIsEnabled(false);
}

void UW_VoicePanel::RefreshChannelList()
{
    if (!Client) {
        return;
    }

    auto OldSelectedOption = VoiceChannelIDComboBox->GetSelectedOption();

    int IndexToSelect = 0;

    VoiceChannelIDComboBox->ClearOptions();
    VoiceChannelIDComboBox->ClearSelection();

    auto AllLobbies = Client->GetLobbyIds();
    for (auto idx = 0; idx < AllLobbies.Num(); ++idx) {
        auto LobbyIDString = AllLobbies[idx].ToString();
        VoiceChannelIDComboBox->AddOption(LobbyIDString);
        if (LobbyIDString == OldSelectedOption) {
            IndexToSelect = idx;
        }
    }

    VoiceChannelIDComboBox->SetSelectedIndex(IndexToSelect);

    if (!Call && AllLobbies.Num() > 0) {
        ConnectButton->SetIsEnabled(true);
    }

    RefreshParticipants();
}

void UW_VoicePanel::OnClickedConnect()
{
    if (!Client) {
        return;
    }

#if PLATFORM_ANDROID
    UAndroidPermissionCallbackProxy::GetInstance()->OnPermissionsGrantedDynamicDelegate.AddDynamic(
      this, &UW_VoicePanel::OnPermissionsGranted);
    UAndroidPermissionFunctionLibrary::AcquirePermissions({"android.permission.RECORD_AUDIO"});
#else
    DoConnect();
#endif
}

void UW_VoicePanel::DoConnect()
{
    FDiscordUniqueID ChannelID{};

    auto SelectedLobby = VoiceChannelIDComboBox->GetSelectedOption();
    auto AllLobbies = Client->GetLobbyIds();
    for (auto idx = 0; idx < AllLobbies.Num(); ++idx) {
        auto LobbyIDString = AllLobbies[idx].ToString();
        if (SelectedLobby == LobbyIDString) {
            ChannelID = AllLobbies[idx];
            break;
        }
    }

    Call = Client->StartCall(ChannelID);
    if (!Call) {
        return;
    }

    ConnectButton->SetIsEnabled(false);
    DisconnectButton->SetIsEnabled(true);
    VoiceChannelIDComboBox->SetIsEnabled(false);

    Call->SetStatusChangedCallback(
      FDiscordCallOnStatusChanged::CreateUObject(this, &UW_VoicePanel::OnCallStatusChanged));
    Call->SetParticipantChangedCallback(
      FDiscordCallOnParticipantChanged::CreateUObject(this, &UW_VoicePanel::OnParticipantChanged));
    Call->SetSpeakingStatusChangedCallback(FDiscordCallOnSpeakingStatusChanged::CreateUObject(
      this, &UW_VoicePanel::OnSpeakingStatusChanged));
    Call->SetOnVoiceStateChangedCallback(
      FDiscordCallOnVoiceStateChanged::CreateUObject(this, &UW_VoicePanel::OnVoiceStateChanged));

    auto DiscordSubsystem = GetOwningLocalPlayer()->GetSubsystem<UDiscordLocalPlayerSubsystem>();
    DiscordSubsystem->OnUserUpdated.AddDynamic(this, &UW_VoicePanel::OnUserChanged);
}

void UW_VoicePanel::OnClickedDisconnect()
{
    if (Client) {
        Client->EndCalls(
          FDiscordClientEndCallsCallback::CreateUObject(this, &UW_VoicePanel::OnCallsEnded));
    }

    DisconnectButton->SetIsEnabled(false);
    Call = nullptr;
}

void UW_VoicePanel::OnCallsEnded()
{
}

void UW_VoicePanel::OnClickedSelfDeaf()
{
    if (!Call) {
        return;
    }

    if (SelfDeafButtonText->GetText().ToString() == "Deafen Me") {
        SelfDeafButtonText->SetText(FText::FromString("Undeafen Me"));
        Call->SetSelfDeaf(true);
    }
    else {
        SelfDeafButtonText->SetText(FText::FromString("Deafen Me"));
        Call->SetSelfDeaf(false);
    }
}

void UW_VoicePanel::OnCallStatusChanged(EDiscordCallStatus Status,
                                        EDiscordCallError Error,
                                        int ErrorCode)
{
    switch (Status) {
    case EDiscordCallStatus::Disconnected:
        ConnectButton->SetIsEnabled(true);
        VoiceChannelIDComboBox->SetIsEnabled(true);
        VoiceStatusText->SetText(FText::FromString("Disconnected"));
        break;
    case EDiscordCallStatus::Joining:
        VoiceStatusText->SetText(FText::FromString("Joining"));
        break;
    case EDiscordCallStatus::Connecting:
        VoiceStatusText->SetText(FText::FromString("Connecting"));
        break;
    case EDiscordCallStatus::SignalingConnected:
        VoiceStatusText->SetText(FText::FromString("SignalingConnected"));
        break;
    case EDiscordCallStatus::Connected:
        VoiceStatusText->SetText(FText::FromString("Connected"));
        break;
    case EDiscordCallStatus::Reconnecting:
        VoiceStatusText->SetText(FText::FromString("Reconnecting"));
        break;
    case EDiscordCallStatus::Disconnecting:
        VoiceStatusText->SetText(FText::FromString("Disconnecting"));
        break;
    };

    FString ErrorMessage;
    switch (Error) {
    case EDiscordCallError::None:
        ErrorMessage.Append("None");
        break;
    case EDiscordCallError::SignalingConnectionFailed:
        ErrorMessage.Append("SignalingConnectionFailed");
        break;
    case EDiscordCallError::SignalingUnexpectedClose:
        ErrorMessage.Append("SignalingUnexpectedClose");
        break;
    case EDiscordCallError::VoiceConnectionFailed:
        ErrorMessage.Append("VoiceConnectionFailed");
        break;
    case EDiscordCallError::JoinTimeout:
        ErrorMessage.Append("JoinTimeout");
        break;
    };
    ErrorMessage.Append(" | ").AppendInt(ErrorCode);
    VoiceErrorText->SetText(FText::FromString(ErrorMessage));

    RefreshParticipants();
}

void UW_VoicePanel::OnParticipantChanged(FDiscordUniqueID UserID, bool Joined)
{
    RefreshParticipants();
}

void UW_VoicePanel::OnSpeakingStatusChanged(FDiscordUniqueID UserID, bool Speaking)
{
    for (int32 idx = 0; idx < ParticipantsList->GetNumItems(); ++idx) {
        auto* listItem = ParticipantsList->GetItemAt(idx);
        auto* wrapper = Cast<UW_VoiceParticipantWrapper>(listItem);
        if (wrapper && wrapper->UserID == UserID) {
            wrapper->Speaking = Speaking;
            break;
        }
    }

    RefreshParticipant(UserID);
}

void UW_VoicePanel::OnVoiceStateChanged(FDiscordUniqueID UserID)
{
    RefreshParticipant(UserID);
}

void UW_VoicePanel::OnUserMuteChanged(FDiscordUniqueID UserID, bool Mute)
{
    RefreshParticipant(UserID);
}

void UW_VoicePanel::OnUserVolumeChanged(FDiscordUniqueID UserID, float Volume)
{
    RefreshParticipant(UserID);
}

void UW_VoicePanel::OnUserChanged(FDiscordUniqueID UserID)
{
    RefreshParticipant(UserID);
}

void UW_VoicePanel::RefreshParticipants()
{
    ParticipantsList->ClearListItems();

    if (!Client || !Call) {
        return;
    }

    TArray<FDiscordUniqueID> Participants{Call->GetParticipants()};

    for (int32 idx = 0; idx < Participants.Num(); ++idx) {
        UW_VoiceParticipantWrapper* wrapper = NewObject<UW_VoiceParticipantWrapper>();
        wrapper->Client = Client;
        wrapper->Call = Call;
        wrapper->UserID = Participants[idx];
        wrapper->Speaking = false;
        ParticipantsList->AddItem(wrapper);
    }
}

void UW_VoicePanel::RefreshParticipant(FDiscordUniqueID UserID)
{
    if (!Client || !Call) {
        return;
    }

    for (int32 idx = 0; idx < ParticipantsList->GetNumItems(); ++idx) {
        auto* listItem = ParticipantsList->GetItemAt(idx);
        auto* wrapper = Cast<UW_VoiceParticipantWrapper>(listItem);
        if (wrapper && wrapper->UserID == UserID) {
            auto* widget =
              Cast<UW_VoiceParticipantEntry>(ParticipantsList->GetEntryWidgetFromItem(listItem));
            if (widget) {
                UE_LOG(
                  DiscordxUnreal, Log, TEXT("Refresh Widget Voice Panel: %s"), *UserID.ToString());
                widget->Refresh();
            }

            break;
        }
    }
}

class UDiscordCall* UW_VoiceParticipantWrapper::GetCall()
{
    return Call.Get();
}

void UW_VoicePanel::OnPermissionsGranted(TArray<FString> const& Permissions,
                                         TArray<bool> const& Granted)
{
#if PLATFORM_ANDROID
    UAndroidPermissionCallbackProxy::GetInstance()->OnPermissionsGrantedDynamicDelegate.RemoveAll(
      this);
    if (Granted[0]) {
        DoConnect();
    }
#endif
}

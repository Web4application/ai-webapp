// Copyright Discord, Inc. All Rights Reserved.

#include "W_UserActionPanel.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

#include "W_FrontEndBase.h"

void UW_UserActionPanel::NativeOnInitialized()
{
    FriendsAcceptButton->OnClicked.AddDynamic(this, &UW_UserActionPanel::OnClickedFriendsAccept);
    FriendsRemoveButton->OnClicked.AddDynamic(this, &UW_UserActionPanel::OnClickedFriendsRemove);
    FriendsBlockButton->OnClicked.AddDynamic(this, &UW_UserActionPanel::OnClickedFriendsBlock);
    RelationshipUpdateCallback.BindDynamic(this, &UW_UserActionPanel::OnRelationshipUpdated);
}

void UW_UserActionPanel::OnUserContextChanged(UW_DiscordUserInfo* UserInfo)
{
    CurrentUserInfo = UserInfo;

    if (!UserInfo) {
        return;
    }

    DisplayNameText->SetText(FText::FromString(UserInfo->User->DisplayName()));

    FriendsAcceptButton->SetVisibility((UserInfo->Relationship->DiscordRelationshipType() ==
                                        EDiscordRelationshipType::PendingIncoming)
                                         ? ESlateVisibility::Visible
                                         : ESlateVisibility::Collapsed);

    FriendsRemoveButton->SetVisibility(ESlateVisibility::Visible);

    FriendsBlockButton->SetVisibility(
      (UserInfo->Relationship->DiscordRelationshipType() != EDiscordRelationshipType::Blocked)
        ? ESlateVisibility::Visible
        : ESlateVisibility::Collapsed);
}

void UW_UserActionPanel::OnRelationshipUpdated(UDiscordClientResult* Result)
{
    FriendsActionSwitcher->SetActiveWidgetIndex(0);
}

void UW_UserActionPanel::OnClickedFriendsAccept()
{
    if (!CurrentUserInfo.IsValid()) {
        return;
    }

    if (!CurrentUserInfo->Client.IsValid()) {
        return;
    }

    FriendsActionSwitcher->SetActiveWidgetIndex(1);

    CurrentUserInfo->Client->AcceptDiscordFriendRequest(CurrentUserInfo->User->Id(),
                                                        RelationshipUpdateCallback);
}

void UW_UserActionPanel::OnClickedFriendsRemove()
{
    if (!CurrentUserInfo.IsValid()) {
        return;
    }

    if (!CurrentUserInfo->Client.IsValid()) {
        return;
    }

    FriendsActionSwitcher->SetActiveWidgetIndex(1);
    CurrentUserInfo->Client->RemoveDiscordAndGameFriend(CurrentUserInfo->User->Id(),
                                                        RelationshipUpdateCallback);
}

void UW_UserActionPanel::OnClickedFriendsBlock()
{
    if (!CurrentUserInfo.IsValid()) {
        return;
    }

    if (!CurrentUserInfo->Client.IsValid()) {
        return;
    }

    FriendsActionSwitcher->SetActiveWidgetIndex(1);
    CurrentUserInfo->Client->BlockUser(CurrentUserInfo->User->Id(), RelationshipUpdateCallback);
}

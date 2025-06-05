"use client";

import {
  LiveblocksProvider,
  RoomProvider,
  ClientSideSuspense,
} from "@liveblocks/react/suspense";
import { CollaborativeEditor } from "./CollaborativeEditor";

export default function App() {
  return (
    <LiveblocksProvider publicApiKey={"pk_prod_9KensGJpifraqw4oar3bVPl2T5Qh5W4NWUpdY0MwaUjA3RMH8gCLjfZyky9lyF77"}>
      <RoomProvider id="my-room">
        <ClientSideSuspense fallback={<div>Loading…</div>}>
          <CollaborativeEditor />
        </ClientSideSuspense>
      </RoomProvider>
    </LiveblocksProvider>
  );
}

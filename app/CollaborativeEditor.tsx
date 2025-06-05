"use client";

import * as Y from "yjs";
import { getYjsProviderForRoom } from "@liveblocks/yjs";
import { useRoom } from "@/liveblocks.config";
import { useCallback, useEffect, useState } from "react";
import { Editor } from "@monaco-editor/react";
import { editor } from "monaco-editor";
import { MonacoBinding } from "y-monaco";
import { Awareness } from "y-protocols/awareness";

// Collaborative text editor with simple rich text, live cursors, and live avatars
export function CollaborativeEditor() {
  const [editorRef, setEditorRef] = useState<editor.IStandaloneCodeEditor>();
  const room = useRoom();
  const yProvider = getYjsProviderForRoom(room);

  // Set up Liveblocks Yjs provider and attach Monaco editor
  useEffect(() => {
    let binding: MonacoBinding;

    if (editorRef) {
      const yDoc = yProvider.getYDoc();
      const yText = yDoc.getText("monaco");

      // Attach Yjs to Monaco
      binding = new MonacoBinding(
        yText,
        editorRef.getModel() as editor.ITextModel,
        new Set([editorRef]),
        yProvider.awareness as Awareness
      );
    }

    return () => {
      binding?.destroy();
    };
  }, [editorRef, room]);

  const handleOnMount = useCallback((e: editor.IStandaloneCodeEditor) => {
    setEditorRef(e);
  }, []);

  return (
    <Editor
      onMount={handleOnMount}
      height="100vh"
      width="100hw"
      theme="vs-light"
      defaultLanguage="typescript"
      defaultValue=""
      options={{
        tabSize: 2,
      }}
    />
  );
}

const { GoogleGenerativeAI } = require("@google/generative-ai");
const speech = require('@google-cloud/speech');
const textToSpeech = require('@google-cloud/text-to-speech');
const fs = require('fs');
const util = require('util');

// Initialize the AI model
const genAI = new GoogleGenerativeAI(process.env. AIzaSyAvrxOyAVzPVcnzxuD0mjKVDyS2bNWfC10);
const model = genAI.getGenerativeModel({ model: "gemini-1.5-flash" });

// Initialize the Speech-to-Text and Text-to-Speech clients
const client = new speech.SpeechClient();
const ttsClient = new textToSpeech.TextToSpeechClient();

// Function to handle the conversation
async function handleConversation(audioFilePath) {
// Convert speech to text
const audio = {
content: fs.readFileSync(audioFilePath).toString('base64'),
};
const request = {
audio: audio,
config: { encoding: 'LINEAR16', sampleRateHertz: 16000, languageCode: 'en-US' },
};
const [response] = await client.recognize(request);
const transcription = response.results.map(result => result.alternatives[0].transcript).join('\n');

// Generate AI response
const prompt = transcription;
const result = await model.generateContent(prompt);
const aiResponse = result.response.text();

// Convert AI response to speech
const ttsRequest = {
input: { text: aiResponse },
voice: { languageCode: 'en-US', ssmlGender: 'NEUTRAL' },
audioConfig: { audioEncoding: 'MP3' },
};
const [ttsResponse] = await ttsClient.synthesizeSpeech(ttsRequest);
const writeFile = util.promisify(fs.writeFile);
await writeFile('output.mp3', ttsResponse.audioContent, 'binary');
console.log('Audio content written to file: output.mp3');
}

// Example usage
handleConversation('input.wav');

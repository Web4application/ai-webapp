import axios from 'axios';

export async function analyzeTranscript(transcriptText) {
  try {
    const { data } = await axios.post('http://localhost:8000/analyze-transcript/', {
      text: transcriptText
    });
    return data.output;
  } catch (err) {
    console.error('Transcript Analysis Error:', err.message);
    return '❌ Error analyzing transcript. Please try again later.';
  }
}

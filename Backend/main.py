# backend/main.py
from fastapi import FastAPI, UploadFile, File, HTTPException
from fastapi.middleware.cors import CORSMiddleware
import openai
import os

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Tighten this in production
    allow_methods=["*"],
    allow_headers=["*"],
)

# Set your OpenAI API key securely via env variable
openai.api_key = os.getenv("OPENAI_API_KEY")

async def analyze_script_with_ai(script_content: str):
    # Here, a simple prompt to analyze dependencies or summarize code
    prompt = (
        "Analyze this JavaScript code for dependencies, execution order, "
        "and potential issues. Provide a structured JSON response.\n\n"
        f"```js\n{script_content}\n```"
    )
    
    response = openai.ChatCompletion.create(
        model="gpt-4o-mini",
        messages=[{"role": "user", "content": prompt}],
        max_tokens=500,
        temperature=0.0,
    )
    
    return response.choices[0].message.content

@app.post("/analyze")
async def analyze_script(script: UploadFile = File(...)):
    if not script.filename.endswith('.js'):
        raise HTTPException(status_code=400, detail="Only .js files allowed")

    content_bytes = await script.read()
    content_str = content_bytes.decode('utf-8')

    # Run your AI analysis
    try:
        ai_analysis = await analyze_script_with_ai(content_str)
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"AI analysis failed: {str(e)}")

    # Return both basic stats and AI analysis result
    return {
        "filename": script.filename,
        "size_kb": round(len(content_bytes) / 1024, 2),
        "num_lines": content_str.count('\n') + 1,
        "ai_analysis": ai_analysis,
    }

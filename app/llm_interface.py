# llm_interface.py
import os
import openai
import yaml

# Load config
with open("config.yaml") as f:
    config = yaml.safe_load(f)

openai.api_key = os.getenv("OPENAI_API_KEY")

def call_model(prompt_text):
    response = openai.ChatCompletion.create(
        model=config["model"],
        messages=[{"role": "user", "content": prompt_text}],
        max_tokens=config["max_tokens"],
        temperature=config["temperature"]
    )
    return response.choices[0].message.content.strip()

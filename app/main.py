from prompts import PROMPTS
from llm_interface import call_model
from preprocessing import clean_text
import yaml
from db import get_connection

def main():
    # Load config for prompt choice
    with open("config.yaml") as f:
        config = yaml.safe_load(f)

    sample_text = """
    Today we discussed project timelines and assigned John to lead the frontend redesign.
    Also, finalize the budget by next Wednesday.
    """

    cleaned_text = clean_text(sample_text)
    prompt_template = PROMPTS[config["prompt_type"]]
    prompt = prompt_template.format(text=cleaned_text)

    response = call_model(prompt)
    print("Model output:\n", response)

    # Save result to DB
    conn = get_connection()
    if conn:
        try:
            cursor = conn.cursor()
            insert_query = "INSERT INTO analysis_results (input_text, output_text) VALUES (%s, %s);"
            cursor.execute(insert_query, (cleaned_text, response))
            conn.commit()
            print("Analysis result saved to database.")
            cursor.close()
        except Exception as e:
            print(f"Failed to save result: {e}")
        finally:
            conn.close()

if __name__ == "__main__":
    main()

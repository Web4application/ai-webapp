# preprocessing.py

import re

def clean_text(text):
    # Basic normalization: remove extra whitespace, special chars
    text = text.strip()
    text = re.sub(r"\s+", " ", text)
    return text

# myapp/views.py
from django.shortcuts import render
from django.http import JsonResponse
import openai
from django.conf import settings

openai.AIzaSyAvrxOyAVzPVcnzxuD0mjKVDyS2bNWfC10 = settings.AIzaSyAvrxOyAVzPVcnzxuD0mjKVDyS2bNWfC10

def generate_text(request):
if request.method == 'POST':
prompt = request.POST.get('prompt')
response = openai.Completion.create(
engine="text-davinci-004",
prompt=prompt,
max_tokens=150
)
return JsonResponse({'text': response.choices[0].text.strip()})
return render(request, 'index.html')

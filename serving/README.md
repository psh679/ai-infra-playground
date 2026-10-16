# Simple FastAPI Serving (AI Infra Day 2)

## Purpose:
- Minimal FastAPI app to demonstrate a service.
- Endpoints:
  - `GET /` -> hello message
  - `GET /health` -> health check
  - `GET /contacts` -> sample contacts JSON

## How to run (Windows):
1. Ensure Python 3.8+ is installed. Check:
python --version
If not installed, download from https://www.python.org/downloads/ and install.

2. Create virtualenv and activate:
python -m venv .venv .venv\Scripts\activate

3. Install dependencies:
pip install -r requirements.txt

4. Run the app:
uvicorn app.main:app --reload --host 127.0.0.1 --port 8000

5. Test endpoints:
- Open http://127.0.0.1:8000 in browser
- Health: http://127.0.0.1:8000/health
- Contacts: http://127.0.0.1:8000/contacts

Notes:
- Later we'll replace SAMPLE_CONTACTS with real data backend (SQLite or your C program via wrapper).


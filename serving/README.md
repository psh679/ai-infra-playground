# Simple FastAPI Serving (AI Infra Day 2)

## Purpose

- Minimal FastAPI app to demonstrate a service
- Exposes contact data through a JSON API
- Uses SQLite as the storage backend instead of in-memory sample data

## Endpoints

- `GET /` -> hello message
- `GET /health` -> health check
- `GET /contacts` -> contact list from SQLite

## How to run locally

### Windows

1. Ensure Python 3.8+ is installed.
   ```bash
   python --version
   ```

2. Create a virtual environment and activate it.
   ```bash
   python -m venv .venv
   .venv\Scripts\activate
   ```

3. Install dependencies.
   ```bash
   pip install -r requirements.txt
   ```

4. Run the app.
   ```bash
   uvicorn app.main:app --reload --host 127.0.0.1 --port 8000
   ```

5. Test endpoints.
   - Open http://127.0.0.1:8000
   - Health: http://127.0.0.1:8000/health
   - Contacts: http://127.0.0.1:8000/contacts

## Docker

```bash
docker build -t ai-infra-playground .
docker run --rm -p 8000:8000 ai-infra-playground
```

## Notes

- The app automatically creates a SQLite database and inserts initial sample records on startup.
- This is a good foundation for later integrating with CSV files, C programs, or real backend services.

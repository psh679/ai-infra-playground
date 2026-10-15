# Simple FastAPI Serving (AI Infra Day 2)

## Purpose

This project is a minimal FastAPI application designed to demonstrate a basic service layer in a modern software architecture.

It includes:

- a lightweight HTTP API
- health checks
- contact data access through SQLite
- a clean foundation for future integration with C-based logic or CSV-based inputs

## Endpoints

- `GET /` -> returns a hello message
- `GET /health` -> returns application health status
- `GET /contacts` -> returns contacts stored in SQLite

## How to run locally

### Windows

1. Ensure Python 3.8+ is installed

```bash
python --version
```

2. Create a virtual environment and activate it

```bash
python -m venv .venv
.venv\Scripts\activate
```

3. Install dependencies

```bash
pip install -r requirements.txt
```

4. Start the server

```bash
uvicorn app.main:app --reload --host 127.0.0.1 --port 8000
```

5. Test the service

- http://127.0.0.1:8000/
- http://127.0.0.1:8000/health
- http://127.0.0.1:8000/contacts

## Docker

You can also run the service in Docker:

```bash
docker build -t ai-infra-playground .
docker run --rm -p 8000:8000 ai-infra-playground
```

## Notes

- The app creates and initializes a SQLite database automatically on startup.
- This is a learning-focused setup intended to be expanded later with real data sources, CSV ingestion, or C program integration.
- The service layer intentionally stays simple so that the architecture is easy to understand and extend.

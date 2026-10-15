# AI Infra Playground

A small learning project for exploring AI infra concepts with a mix of C-based data logic and a Python serving layer.

## Repository layout

- `contacts/` – C project for contact-related logic and data handling
- `serving/` – FastAPI service exposing the contacts data over HTTP
- `examples/` – experiments and small demos
- `README.md` – repo overview

## Current status

The project currently contains:

- a C-based contact app structure
- a FastAPI service running on port 8000
- SQLite-backed contact persistence in `serving/app/contacts.db`
- a simple health endpoint and contact API

## Quick start

### Start the API locally

```bash
cd serving
python -m venv .venv
source .venv/bin/activate  # Windows: .venv\Scripts\activate
pip install -r requirements.txt
uvicorn app.main:app --host 0.0.0.0 --port 8000 --reload
```

Then visit:

- http://127.0.0.1:8000/
- http://127.0.0.1:8000/health
- http://127.0.0.1:8000/contacts

## Docker

```bash
cd serving
docker build -t ai-infra-playground .
docker run --rm -p 8000:8000 ai-infra-playground
```

## Learning goals

- Build a minimal backend service
- Store data in a real database instead of in-memory structures
- Practice using a service layer on top of a C-based project
- Move toward a future AI infra architecture with deployment, observability, and automation

## Next ideas

- add Docker Compose
- add GitHub Actions CI
- add metrics/health UI
- integrate CSV ingestion or C wrapper data source
- deploy to Kubernetes

# AI Infra Playground

A lightweight learning project for exploring a simple AI infrastructure stack using:

- a C-based logic/data layer
- a Python FastAPI service layer
- SQLite persistence
- Docker-based deployment

This project is intentionally small and easy to run, so it can be used as a practical example of how a minimal service architecture is structured in real-world software systems.

## Why this project exists

The goal is to demonstrate a basic pattern that appears often in backend and infrastructure work:

- low-level logic and business rules live in a separate component
- HTTP API is exposed through a service layer
- state is stored in a persistent database
- deployment is handled with a container runtime

This project is designed to help learn how to connect a compiled C component with a modern Python web service and how a small service can become a foundation for larger infra patterns.

## Architecture overview

The repository currently follows a simple layered architecture:

- C layer: handles contact-related logic and foundational data operations
- Python/FastAPI layer: serves the API over HTTP
- SQLite database: stores records persistently
- Docker: packages the service for reproducible local deployment

This is a minimal example of a larger system pattern where:

- application logic is separated from transport and presentation
- data is managed independently from the API layer
- services can later evolve into more complex workflows

## Repository layout

```text
ai-infra-playground/
├── README.md
├── contacts/               # C-based data and logic code
├── serving/                # FastAPI service code and configuration
│   ├── app/
│   ├── requirements.txt
│   ├── Dockerfile
│   ├── README.md
│   └── .dockerignore
├── examples/               # demos and experiments
└── ...
```

## Current service behavior

The FastAPI service currently provides the following endpoints:

- `GET /` -> returns a welcome message
- `GET /health` -> health check
- `GET /contacts` -> returns contacts stored in SQLite

## Local development

### 1. Create a virtual environment

```bash
cd serving
python -m venv .venv
```

On Windows:

```bash
.venv\Scripts\activate
```

On macOS / Linux:

```bash
source .venv/bin/activate
```

### 2. Install dependencies

```bash
pip install -r requirements.txt
```

### 3. Start the server

```bash
uvicorn app.main:app --host 0.0.0.0 --port 8000 --reload
```

### 4. Test the API

Open these URLs in a browser or API client:

- http://127.0.0.1:8000/
- http://127.0.0.1:8000/health
- http://127.0.0.1:8000/contacts

## Docker

You can also run the service in a container:

```bash
docker build -t ai-infra-playground .
docker run --rm -p 8000:8000 ai-infra-playground
```

Then test:

- http://127.0.0.1:8000/
- http://127.0.0.1:8000/health
- http://127.0.0.1:8000/contacts

## Example response

Example output from `GET /contacts`:

```json
[
  {
    "id": 1,
    "name": "Alice",
    "email": "alice@example.com",
    "phone": "123-456-7890"
  },
  {
    "id": 2,
    "name": "Bob",
    "email": "bob@example.com",
    "phone": "987-654-3210"
  }
]
```

## Why this matters

This project demonstrates several important software and infra principles:

- application logic can be decoupled from the network layer
- a lightweight persistence layer is enough for early-stage services
- a service can be exposed through HTTP without exposing internal implementation details
- containerization makes local and cloud deployments more consistent

This is especially useful for learning how larger systems are built, where components often include data layers, web services, orchestration, monitoring, and deployment automation.

## Possible next steps

The project can evolve in many useful directions:

- integrate CSV ingestion
- add a wrapper around the C program
- add POST /contacts and delete operations
- switch from SQLite to a heavier database in later stages
- add Docker Compose
- add CI with GitHub Actions
- add monitoring and health dashboards
- prepare for Kubernetes deployment

## Summary

This repository is a minimal but practical example of a small service architecture:

- C for low-level logic
- Python/FastAPI for API exposure
- SQLite for persistence
- Docker for easy deployment

It is a useful starting point for learning service composition, backend architecture, and infra-oriented thinking in a compact project.

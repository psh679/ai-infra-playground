from fastapi import FastAPI
from pydantic import BaseModel
from typing import List
from .db import init_db, get_all_contacts

app = FastAPI(title="AI Infra Playground - Simple Serving")

class Contact(BaseModel):
    id: int
    name: str
    phone: str
    email: str
    address: str = ""
    note: str = ""

@app.on_event("startup")
def startup_event():
    # Initialize SQLite DB and insert sample data if needed
    init_db()

@app.get("/")
def root():
    return {"message": "Hello AI infra — from FastAPI"}

@app.get("/health")
def health():
    return {"status": "ok"}

@app.get("/contacts", response_model=List[Contact])
def list_contacts():
    # Read contacts from SQLite DB
    rows = get_all_contacts()
    return rows

from fastapi import FastAPI, HTTPException, status, Query, Response
from pydantic import BaseModel
from typing import List, Optional

from app.db import create_contact, get_contacts, get_contact_by_id, delete_contact, init_db

app = FastAPI(title="AI Infra Playground - Simple Serving")


class Contact(BaseModel):
    id: int
    name: str
    phone: str
    email: str
    address: str = ""
    note: str = ""


class ContactCreate(BaseModel):
    name: str
    phone: str
    email: str
    address: str = ""
    note: str = ""


@app.on_event("startup")
def startup_event():
    init_db()


@app.get("/")
def root():
    return {"message": "Hello AI infra — from FastAPI"}


@app.get("/health")
def health():
    return {"status": "ok"}


@app.get("/contacts", response_model=List[Contact])
def list_contacts(
    limit: int = Query(100, ge=1, le=1000),
    offset: int = Query(0, ge=0),
    q: Optional[str] = Query(None, description="Filter by name or email (substring match)"),
):
    return get_contacts(limit=limit, offset=offset, q=q)


@app.get("/contacts/{contact_id}", response_model=Contact)
def get_contact(contact_id: int):
    contact = get_contact_by_id(contact_id)
    if contact is None:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="contact not found")
    return contact


@app.post("/contacts", response_model=Contact, status_code=status.HTTP_201_CREATED)
def create_contact_endpoint(contact: ContactCreate):
    try:
        return create_contact(contact.model_dump())
    except ValueError as exc:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail=str(exc)) from exc


@app.delete("/contacts/{contact_id}", status_code=status.HTTP_204_NO_CONTENT)
def delete_contact_endpoint(contact_id: int):
    ok = delete_contact(contact_id)
    if not ok:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="contact not found")
    return Response(status_code=status.HTTP_204_NO_CONTENT)


if __name__ == "__main__":
    import uvicorn

    uvicorn.run(app, host="0.0.0.0", port=8000)

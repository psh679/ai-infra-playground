from fastapi import FastAPI
from pydantic import BaseModel
from typing import List
app = FastAPI(title="AI Infra Playground - Simple Serving")
class Contact(BaseModel):
    id: int
    name: str
    phone: str
    email: str
    address: str = ""
    note: str = ""
# 示例数据（后面我们会把它替换成从 SQLite / 实际服务读取）
SAMPLE_CONTACTS: List[Contact] = [
    Contact(id=1, name="Alice", phone="13800138000", email="alice@example.com", address="Beijing", note="Friend"),
    Contact(id=2, name="Bob", phone="+86-13900000000", email="bob@example.com", address="Shanghai", note="Colleague"),
]
@app.get("/")
def root():
    return {"message": "Hello AI infra — from FastAPI"}
@app.get("/health")
def health():
    return {"status": "ok"}
@app.get("/contacts", response_model=List[Contact])
def list_contacts():
    # 将来改为从数据库或模型接口读取
    return SAMPLE_CONTACTS

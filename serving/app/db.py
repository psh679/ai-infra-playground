from pathlib import Path
import sqlite3
from typing import List, Dict

DB_PATH = Path(__file__).resolve().parent / "contacts.db"


def get_conn():
    conn = sqlite3.connect(str(DB_PATH))
    conn.row_factory = sqlite3.Row
    return conn


def init_db():
    DB_PATH.parent.mkdir(parents=True, exist_ok=True)
    conn = get_conn()
    cur = conn.cursor()
    cur.execute("""
    CREATE TABLE IF NOT EXISTS contacts (
        id INTEGER PRIMARY KEY,
        name TEXT NOT NULL,
        phone TEXT NOT NULL,
        email TEXT NOT NULL,
        address TEXT DEFAULT '',
        note TEXT DEFAULT ''
    )
    """)
    conn.commit()
    cur.execute("SELECT COUNT(*) as cnt FROM contacts")
    row = cur.fetchone()
    cnt = row[0] if row is not None else 0
    if cnt == 0:
        cur.executemany(
            "INSERT INTO contacts (id, name, phone, email, address, note) VALUES (?, ?, ?, ?, ?, ?)",
            [
                (1, "Alice", "13800138000", "alice@example.com", "Beijing", "Friend"),
                (2, "Bob", "+86-13900000000", "bob@example.com", "Shanghai", "Colleague"),
            ]
        )
        conn.commit()
    conn.close()


def get_all_contacts() -> List[Dict]:
    conn = get_conn()
    cur = conn.cursor()
    cur.execute("SELECT id, name, phone, email, address, note FROM contacts ORDER BY id")
    rows = cur.fetchall()
    conn.close()
    return [dict(row) for row in rows]

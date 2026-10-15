from pathlib import Path
import csv
import sqlite3
from typing import Any, Dict, List, Optional

DB_PATH = Path(__file__).resolve().parent / "contacts.db"
CSV_PATH = Path(__file__).resolve().parents[2] / "contacts" / "data" / "contacts.csv"


def get_conn():
    conn = sqlite3.connect(str(DB_PATH))
    conn.row_factory = sqlite3.Row
    return conn


def import_contacts_from_csv():
    if not CSV_PATH.exists():
        return False

    rows = []
    with CSV_PATH.open("r", newline="", encoding="utf-8") as csv_file:
        reader = csv.DictReader(csv_file)
        for row in reader:
            try:
                _id = int(row.get("id", 0)) if row.get("id") else None
            except ValueError:
                _id = None
            rows.append(
                (
                    _id,
                    row.get("name", "").strip(),
                    row.get("phone", "").strip(),
                    row.get("email", "").strip(),
                    row.get("address", "").strip(),
                    row.get("note", "").strip(),
                )
            )

    if not rows:
        return False

    conn = get_conn()
    cur = conn.cursor()

    # If CSV provides ids, use INSERT OR REPLACE to avoid conflicts
    cur.executemany(
        "INSERT OR REPLACE INTO contacts (id, name, phone, email, address, note) VALUES (?, ?, ?, ?, ?, ?)",
        rows,
    )
    conn.commit()
    conn.close()
    return True


def init_db():
    DB_PATH.parent.mkdir(parents=True, exist_ok=True)
    conn = get_conn()
    cur = conn.cursor()
    cur.execute(
        """
        CREATE TABLE IF NOT EXISTS contacts (
            id INTEGER PRIMARY KEY,
            name TEXT NOT NULL,
            phone TEXT NOT NULL,
            email TEXT NOT NULL,
            address TEXT DEFAULT '',
            note TEXT DEFAULT ''
        )
        """
    )
    conn.commit()

    cur.execute("SELECT COUNT(*) as cnt FROM contacts")
    row = cur.fetchone()
    cnt = row[0] if row is not None else 0

    if cnt == 0:
        if CSV_PATH.exists():
            import_contacts_from_csv()
        else:
            cur.executemany(
                "INSERT INTO contacts (id, name, phone, email, address, note) VALUES (?, ?, ?, ?, ?, ?)",
                [
                    (1, "Alice", "13800138000", "alice@example.com", "Beijing", "Friend"),
                    (2, "Bob", "+86-13900000000", "bob@example.com", "Shanghai", "Colleague"),
                ],
            )
            conn.commit()

    conn.close()


def create_contact(payload: Dict[str, Any]) -> Dict[str, Any]:
    name = (payload.get("name") or "").strip()
    phone = (payload.get("phone") or "").strip()
    email = (payload.get("email") or "").strip()
    address = (payload.get("address") or "").strip()
    note = (payload.get("note") or "").strip()

    if not name or not phone or not email:
        raise ValueError("name, phone, and email are required")

    conn = get_conn()
    cur = conn.cursor()
    cur.execute(
        """
        INSERT INTO contacts (name, phone, email, address, note)
        VALUES (?, ?, ?, ?, ?)
        """,
        (name, phone, email, address, note),
    )
    conn.commit()
    contact_id = cur.lastrowid
    conn.close()

    return {
        "id": contact_id,
        "name": name,
        "phone": phone,
        "email": email,
        "address": address,
        "note": note,
    }


def get_contact_by_id(contact_id: int) -> Optional[Dict[str, Any]]:
    conn = get_conn()
    cur = conn.cursor()
    cur.execute(
        "SELECT id, name, phone, email, address, note FROM contacts WHERE id = ?",
        (contact_id,),
    )
    row = cur.fetchone()
    conn.close()
    return dict(row) if row is not None else None


def delete_contact(contact_id: int) -> bool:
    conn = get_conn()
    cur = conn.cursor()
    cur.execute("DELETE FROM contacts WHERE id = ?", (contact_id,))
    conn.commit()
    deleted = cur.rowcount > 0
    conn.close()
    return deleted


def get_contacts(limit: int = 100, offset: int = 0, q: Optional[str] = None) -> List[Dict[str, Any]]:
    conn = get_conn()
    cur = conn.cursor()
    if q:
        like_q = f"%{q}%"
        cur.execute(
            "SELECT id, name, phone, email, address, note FROM contacts WHERE name LIKE ? OR email LIKE ? ORDER BY id LIMIT ? OFFSET ?",
            (like_q, like_q, limit, offset),
        )
    else:
        cur.execute(
            "SELECT id, name, phone, email, address, note FROM contacts ORDER BY id LIMIT ? OFFSET ?",
            (limit, offset),
        )
    rows = cur.fetchall()
    conn.close()
    return [dict(row) for row in rows]

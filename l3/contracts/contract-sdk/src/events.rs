//! Emisión de eventos indexables.
use serde::{Deserialize, Serialize};
use std::sync::{Arc, Mutex};

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Event {
    pub name: String,
    pub topics: Vec<String>,
    pub data: serde_json::Value,
}

#[derive(Clone, Default)]
pub struct EventLog {
    events: Arc<Mutex<Vec<Event>>>,
}

impl EventLog {
    pub fn emit(&self, name: &str, topics: Vec<String>, data: serde_json::Value) {
        self.events.lock().unwrap().push(Event {
            name: name.into(),
            topics,
            data,
        });
    }

    pub fn all(&self) -> Vec<Event> {
        self.events.lock().unwrap().clone()
    }

    pub fn clear(&self) {
        self.events.lock().unwrap().clear();
    }
}

# FAQ

Q: Why components over inheritance?
A: Components allow feature reuse and reduce tight coupling, enabling focused testing and iteration.

Q: Why avoid Tick by default?
A: Event-driven design reduces per-frame cost and complexity; use timers, delegates, BT services.

Q: Where should I store balancing parameters?
A: Primary Data Assets. They’re editable, serializable, and don’t require recompiles.

Q: Do I need Gameplay Tags?
A: Optional. They help with categorization and conditional logic; keep usage narrow initially.

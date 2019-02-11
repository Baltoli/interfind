# InterFind Code Style

This document collects notes about the coding style I will aim to use when
developing InterFind.

## General

* Use LLVM streams for output for consistency, but perform construction and
  formatting of messages using fmt to avoid huge sequences of stream operators.

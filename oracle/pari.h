#pragma once
#include <optional>
#include <string>

// PARI/GP oracle adapter (ARCHITECTURE.md §4). This lives in oracle/, never in
// src/: oracles referee our numbers, they do not compute them. gp is invoked as
// a subprocess; it is never linked. A missing gp is surfaced (find_gp returns
// nullopt) so the caller can SKIP visibly rather than fail.
namespace oracle {

// Absolute path to the `gp` executable if it is found on $PATH; nullopt if not.
std::optional<std::string> find_gp();

// Run a GP script (a batch of statements) through `gp -q` and return its stdout.
// One process spawn per batch, not per check (ARCHITECTURE.md §4).
// Precondition: gp_path is a valid gp executable (see find_gp).
// Throws std::runtime_error if the subprocess cannot be run or exits nonzero.
std::string run_gp(const std::string& gp_path, const std::string& script);

}  // namespace oracle

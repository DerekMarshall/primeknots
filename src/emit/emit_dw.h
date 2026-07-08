#pragma once
#include <string>

// Stage 6 emitter (ARCHITECTURE §5). Reads the PARI referee cache of S3 cubic
// iso-class counts and writes dw_s3.json: per prime set S, the assembled
// |Hom(π₁,S₃)| and Z_DW (exact rational) with the image-decomposition terms and
// signature note; aggregate N_S₃ distribution vs t. The cache path is a plain
// data file (src/ stays oracle-free).
namespace at::emit {

void emit_stage6(const std::string& out_dir, const std::string& cubic_cache_path,
                 const std::string& generated_by);

}  // namespace at::emit

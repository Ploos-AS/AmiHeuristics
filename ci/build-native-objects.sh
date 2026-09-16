#!/usr/bin/env bash
set -euo pipefail

IMAGE="${AMIHEUR_BEBBO_IMAGE:-amigadev/m68k-amigaos-gcc@sha256:b18080e6ffca8f793e0f539536a9138e9d2a548ca1a301c7483f43ee15fedfed}"
OUT="${1:-build/native}"
mkdir -p "$OUT"

docker pull "$IMAGE"
docker image inspect "$IMAGE" --format '{{join .RepoDigests "\n"}}' | tee "$OUT/toolchain-image.txt"

compile() {
  src="$1"
  obj="$2"
  docker run --rm \
    -v "$PWD:/work" \
    -w /work \
    "$IMAGE" \
    m68k-amigaos-gcc \
      -Iinclude \
      -DAMIHEURISTICS_AMIGA_NATIVE=1 \
      -std=c89 -Wall -Wextra -Werror -pedantic \
      -m68000 -msoft-float \
      -c "$src" -o "$obj"
}

compile src/amiga/inventory_native.c "$OUT/inventory_native.o"
compile src/amiga/vector_reader_native.c "$OUT/vector_reader_native.o"
compile src/amiga/code_regions.c "$OUT/code_regions.o"

file "$OUT"/*.o | tee "$OUT/file.txt"
sha256sum "$OUT"/*.o | tee "$OUT/objects.sha256"

if ! grep -Eiq 'm68k|68000|Amiga' "$OUT/file.txt"; then
  echo "ERROR: native objects are not recognized as m68k/Amiga objects" >&2
  exit 1
fi

printf 'STATUS=PASS\nGATE=M3_17_NATIVE_BEBBO_COMPILE\nIMAGE=%s\n' "$IMAGE" | tee "$OUT/result.txt"

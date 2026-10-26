#!/bin/sh
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$root"

: "${CLOUDFLARE_API_TOKEN:?set CLOUDFLARE_API_TOKEN before deploying}"
node site/build.mjs
node --test site/build.test.mjs
npx --yes wrangler deploy

# Publishing (no-friction)

You can prep everything **without signing in**.  
To actually upload/publish on GitHub, you’ll need to sign in once.

## Option A — Just use GitHub as a doc hub (fastest)
1. Create a repo
2. Upload everything in this folder
3. Your **README.md** + `docs/` are immediately browsable

## Option B — Turn it into a real docs website (MkDocs → GitHub Pages)
### Local preview (optional)
- Install Python 3
- `pip install mkdocs-material`
- From repo root: `mkdocs serve`
- Open the local URL it prints

### Publish on GitHub Pages
- Add a GitHub Actions workflow (Material/MkDocs has common templates)
- Or build locally: `mkdocs build` then push the `site/` folder to Pages

## Option C — GitHub Wiki
- In your repo, enable Wiki
- Copy/paste the files from `wiki/` into the Wiki pages

## Option D — Slides
- Reveal.js: publish `slides/reveal/` as a Pages site (or open locally)
- Marp: open `slides/marp_deck.md` in VS Code + Marp extension and export PDF/HTML

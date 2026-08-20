# Third-Party Notices

This firmware vendors the following third-party source code:

## cJSON (`components/cJSON`)

- Source: https://github.com/DaveGamble/cJSON
- License: MIT (see `components/cJSON/LICENSE`)

## seeed_epaper (`components/seeed_epaper`) and debug_logging (`components/debug_logging`)

- Source: [`sticky-2048`](https://github.com/Lukilyy/reterminal-sticky-2048-eink-game)
  integration in the
  [reTerminal Sticky Playground registry](https://github.com/Seeed-Projects/reterminal-sticky-playground-registry),
  by [Lukilyy](https://github.com/Lukilyy).
- License: MIT. The upstream project's `LICENSE` file text:

  ```
  MIT License

  Copyright (c) 2026 Lukilyy

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  ```

## font8x8_basic (`main/font8x8_basic.h`)

- Source: https://github.com/dhepper/font8x8
- License: Public Domain

## Card and token data

Card data displayed by this firmware is fetched at runtime from the
[Scryfall API](https://scryfall.com/docs/api) and is not redistributed as part
of this repository. Magic: The Gathering is a trademark of Wizards of the
Coast. This project is not produced by, endorsed by, or affiliated with
Wizards of the Coast.

describe('Paragraph Behavior', function() {
    let container;

    let notoSerifFontBuffer = null;
    // This font is known to support kerning
    const notoSerifFontLoaded = fetch('/assets/NotoSerif-Regular.ttf').then(
        (response) => response.arrayBuffer()).then(
        (buffer) => {
            notoSerifFontBuffer = buffer;
        });

    let notoSerifBoldItalicFontBuffer = null;
    const notoSerifBoldItalicFontLoaded = fetch('/assets/NotoSerif-BoldItalic.ttf').then(
        (response) => response.arrayBuffer()).then(
        (buffer) => {
            notoSerifBoldItalicFontBuffer = buffer;
        });

    let emojiFontBuffer = null;
    const emojiFontLoaded = fetch('/assets/NotoColorEmoji.ttf').then(
        (response) => response.arrayBuffer()).then(
        (buffer) => {
            emojiFontBuffer = buffer;
        });

    beforeEach(async () => {
        await LoadCanvasKit;
        await notoSerifFontLoaded;
        await notoSerifBoldItalicFontLoaded;
        await emojiFontLoaded;
        container = document.createElement('div');
        container.innerHTML = `
            <canvas width=600 height=600 id=test></canvas>
            <canvas width=600 height=600 id=report></canvas>`;
        document.body.appendChild(container);
    });

    afterEach(() => {
        document.body.removeChild(container);
    });

    gm('paragraph_basic', (canvas) => {
        const paint = new CanvasKit.SkPaint();

        paint.setColor(CanvasKit.RED);
        paint.setStyle(CanvasKit.PaintStyle.Stroke);

        const fontMgr = CanvasKit.SkFontMgr.FromData(notoSerifFontBuffer);
        expect(fontMgr.countFamilies()).toEqual(1);
        expect(fontMgr.getFamilyName(0)).toEqual('Noto Serif');

        const wrapTo = 200;

        const paraStyle = new CanvasKit.ParagraphStyle({
            textStyle: {
                color: CanvasKit.BLACK,
                fontFamilies: ['Noto Serif'],
                fontSize: 20,
            },
            textAlign: CanvasKit.TextAlign.Center,
            maxLines: 8,
            ellipsis: '.._.',
        });

        const builder = CanvasKit.ParagraphBuilder.Make(paraStyle, fontMgr);
        builder.addText('VAVAVAVAVAVAVA\nVAVA\n');

        const blueText = new CanvasKit.TextStyle({
            backgroundColor: CanvasKit.Color(234, 208, 232), // light pink
            color: CanvasKit.Color(48, 37, 199),
            fontFamilies: ['Noto Serif'],
            decoration: CanvasKit.LineThroughDecoration,
            decorationThickness: 1.5, // multiplier based on font size
            fontSize: 24,
        });
        builder.pushStyle(blueText);
        builder.addText(`Gosh I hope this wraps at some point, it is such a long line.`)
        builder.pop();
        builder.addText(` I'm done with the blue now. `)
        builder.addText(`Now I hope we should stop before we get 8 lines tall. `);
        const paragraph = builder.build();

        paragraph.layout(wrapTo);

        expect(paragraph.didExceedMaxLines()).toBeTruthy();
        expect(paragraph.getAlphabeticBaseline()).toBeCloseTo(21.377, 3);
        expect(paragraph.getHeight()).toEqual(240);
        expect(paragraph.getIdeographicBaseline()).toBeCloseTo(27.236, 3);
        expect(paragraph.getLongestLine()).toBeCloseTo(193.820, 3);
        expect(paragraph.getMaxIntrinsicWidth()).toBeCloseTo(1444.250, 3);
        expect(paragraph.getMaxWidth()).toEqual(200);
        expect(paragraph.getMinIntrinsicWidth()).toBeCloseTo(172.360, 3);
        expect(paragraph.getWordBoundary(8)).toEqual({
            start: 0,
            end: 14,
        });
        expect(paragraph.getWordBoundary(25)).toEqual({
            start: 25,
            end: 26,
        });

        canvas.drawRect(CanvasKit.LTRBRect(10, 10, wrapTo+10, 230), paint);
        canvas.drawParagraph(paragraph, 10, 10);

        paint.delete();
        fontMgr.delete();
        paragraph.delete();
        builder.delete();
    });

    gm('paragraph_foreground_and_background_color', (canvas) => {
        const paint = new CanvasKit.SkPaint();

        paint.setColor(CanvasKit.RED);
        paint.setStyle(CanvasKit.PaintStyle.Stroke);

        const fontMgr = CanvasKit.SkFontMgr.FromData(notoSerifFontBuffer);
        expect(fontMgr.countFamilies()).toEqual(1);
        expect(fontMgr.getFamilyName(0)).toEqual('Noto Serif');

        const wrapTo = 200;

        const paraStyle = new CanvasKit.ParagraphStyle({
            textStyle: {
                foregroundColor: CanvasKit.Color4f(1.0, 0, 0, 0.8),
                backgroundColor: CanvasKit.Color4f(0, 0, 1.0, 0.8),
                // color should default to black
                fontFamilies: ['Noto Serif'],
                fontSize: 20,
            },

            textAlign: CanvasKit.TextAlign.Center,
        });
        const builder = CanvasKit.ParagraphBuilder.Make(paraStyle, fontMgr);
        builder.addText(
            'This text has a red foregroundColor and a blue backgroundColor.');
        const paragraph = builder.build();
        paragraph.layout(300);

        canvas.drawParagraph(paragraph, 10, 10);

        paint.delete();
        fontMgr.delete();
        paragraph.delete();
        builder.delete();
    });

    // loosely based on SkParagraph_GetRectsForRangeParagraph test in c++ code.
    gm('paragraph_rects', (canvas) => {
        const fontMgr = CanvasKit.SkFontMgr.FromData(notoSerifFontBuffer);

        const wrapTo = 550;
        const hStyle = CanvasKit.RectHeightStyle.Max;
        const wStyle = CanvasKit.RectWidthStyle.Tight;

        const mallocedColor = CanvasKit.Malloc(Float32Array, 4);
        mallocedColor.toTypedArray().set([0.9, 0.1, 0.1, 1.0]);

        const paraStyle = new CanvasKit.ParagraphStyle({
            textStyle: {
                color: mallocedColor,
                fontFamilies: ['Noto Serif'],
                fontSize: 50,
            },
            textAlign: CanvasKit.TextAlign.Left,
            maxLines: 10,
        });
        const builder = CanvasKit.ParagraphBuilder.Make(paraStyle, fontMgr);
        builder.addText('12345,  \"67890\" 12345 67890 12345 67890 12345 67890 12345 67890 12345 67890 12345');
        const paragraph = builder.build();
        CanvasKit.Free(mallocedColor);

        paragraph.layout(wrapTo);

        const ranges = [
            {
                start: 0,
                end: 0,
                expectedNum: 0,
            },
            {
                start: 0,
                end: 1,
                expectedNum: 1,
                color: CanvasKit.Color(200, 0, 200),
            },
            {
                start: 2,
                end: 8,
                expectedNum: 1,
                color: CanvasKit.Color(255, 0, 0),
            },
            {
                start: 8,
                end: 21,
                expectedNum: 1,
                color: CanvasKit.Color(0, 255, 0),
            },
            {
                start: 30,
                end: 100,
                expectedNum: 4,
                color: CanvasKit.Color(0, 0, 255),
            },
            {
                start: 19,
                end: 22,
                expectedNum: 1,
                color: CanvasKit.Color(0, 200, 200),
            }
        ];
        // Move it down a bit so we can see the rects that go above 0,0
        canvas.translate(10, 10);
        canvas.drawParagraph(paragraph, 0, 0);

        for (const test of ranges) {
            let rects = paragraph.getRectsForRange(test.start, test.end, hStyle, wStyle);
            expect(Array.isArray(rects)).toEqual(true);
            expect(rects.length).toEqual(test.expectedNum);

            for (const rect of rects) {
                expect(rect.direction.value).toEqual(CanvasKit.TextDirection.LTR.value);
                const p = new CanvasKit.SkPaint();
                p.setColor(test.color);
                p.setStyle(CanvasKit.PaintStyle.Stroke);
                canvas.drawRect(rect, p);
                p.delete();
            }
        }

        fontMgr.delete();
        paragraph.delete();
        builder.delete();
    });

    gm('paragraph_emoji', (canvas) => {
        const fontMgr = CanvasKit.SkFontMgr.FromData([notoSerifFontBuffer, emojiFontBuffer]);
        expect(fontMgr.countFamilies()).toEqual(2);
        expect(fontMgr.getFamilyName(0)).toEqual('Noto Serif');
        expect(fontMgr.getFamilyName(1)).toEqual('Noto Color Emoji');

        const wrapTo = 450;

        const paraStyle = new CanvasKit.ParagraphStyle({
            textStyle: {
                color: CanvasKit.BLACK,
                // Put text first, otherwise the "emoji space" is used and that looks bad.
                fontFamilies: ['Noto Serif', 'Noto Color Emoji'],
                fontSize: 30,
            },
            textAlign: CanvasKit.TextAlign.Left,
            maxLines: 10,
        });

        const textStyle = new CanvasKit.TextStyle({
            color: CanvasKit.BLACK,
            // The number 4 matches an emoji and looks strange w/o this additional style.
            fontFamilies: ['Noto Serif'],
            fontSize: 30,
        });

        const builder = CanvasKit.ParagraphBuilder.Make(paraStyle, fontMgr);
        builder.pushStyle(textStyle);
        builder.addText('4 flags on following line:\n');
        builder.pop();
        builder.addText(`?????????????? ???????? ???????? ????????\n`);
        builder.addText('Rainbow Italy Liberia USA\n\n');
        builder.addText('Emoji below should wrap:\n');
        builder.addText(`?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????`);
        const paragraph = builder.build();

        paragraph.layout(wrapTo);

        canvas.drawParagraph(paragraph, 10, 10);

        const paint = new CanvasKit.SkPaint();
        paint.setColor(CanvasKit.RED);
        paint.setStyle(CanvasKit.PaintStyle.Stroke);
        canvas.drawRect(CanvasKit.LTRBRect(10, 10, wrapTo+10, wrapTo+10), paint);

        fontMgr.delete();
        paint.delete();
        builder.delete();
        paragraph.delete();
    });

    gm('paragraph_hits', (canvas) => {
        const fontMgr = CanvasKit.SkFontMgr.FromData([notoSerifFontBuffer]);

        const wrapTo = 300;

        const paraStyle = new CanvasKit.ParagraphStyle({
            textStyle: {
                color: CanvasKit.BLACK,
                fontFamilies: ['Noto Serif'],
                fontSize: 50,
            },
            textAlign: CanvasKit.TextAlign.Left,
            maxLines: 10,
        });
        const builder = CanvasKit.ParagraphBuilder.Make(paraStyle, fontMgr);
        builder.addText('UNCOPYRIGHTABLE');
        const paragraph = builder.build();

        paragraph.layout(wrapTo);

        canvas.translate(10, 10);
        canvas.drawParagraph(paragraph, 0, 0);

        const paint = new CanvasKit.SkPaint();

        paint.setColor(CanvasKit.Color(255, 0, 0));
        paint.setStyle(CanvasKit.PaintStyle.Fill);
        canvas.drawCircle(20, 30, 3, paint);

        paint.setColor(CanvasKit.Color(0, 0, 255));
        canvas.drawCircle(80, 90, 3, paint);

        paint.setColor(CanvasKit.Color(0, 255, 0));
        canvas.drawCircle(280, 2, 3, paint);

        let posU = paragraph.getGlyphPositionAtCoordinate(20, 30);
        expect(posU).toEqual({
            pos: 1,
            affinity: CanvasKit.Affinity.Upstream
        });
        let posA = paragraph.getGlyphPositionAtCoordinate(80, 90);
        expect(posA).toEqual({
            pos: 11,
            affinity: CanvasKit.Affinity.Downstream
        });
        let posG = paragraph.getGlyphPositionAtCoordinate(280, 2);
        expect(posG).toEqual({
            pos: 9,
            affinity: CanvasKit.Affinity.Upstream
        });

        builder.delete();
        paragraph.delete();
        paint.delete();
        fontMgr.delete();
    });

    gm('paragraph_styles', (canvas) => {
        const paint = new CanvasKit.SkPaint();

        paint.setColor(CanvasKit.RED);
        paint.setStyle(CanvasKit.PaintStyle.Stroke);

        const fontMgr = CanvasKit.SkFontMgr.FromData(notoSerifFontBuffer, notoSerifBoldItalicFontBuffer);

        const wrapTo = 250;

        const paraStyle = new CanvasKit.ParagraphStyle({
            textStyle: {
                fontFamilies: ['Noto Serif'],
                fontSize: 20,
                fontStyle: {
                    weight: CanvasKit.FontWeight.Light,
                }
            },
            textDirection: CanvasKit.TextDirection.RTL,
            disableHinting: true,
        });

        const builder = CanvasKit.ParagraphBuilder.Make(paraStyle, fontMgr);
        builder.addText('Default text\n');

        const boldItalic = new CanvasKit.TextStyle({
            color: CanvasKit.RED,
            fontFamilies: ['Noto Serif'],
            fontSize: 20,
            fontStyle: {
                weight: CanvasKit.FontWeight.Bold,
                width: CanvasKit.FontWidth.Expanded,
                slant: CanvasKit.FontSlant.Italic,
            }
        });
        builder.pushStyle(boldItalic);
        builder.addText(`Bold, Expanded, Italic\n`);
        builder.pop();
        builder.addText(`back to normal`);
        const paragraph = builder.build();

        paragraph.layout(wrapTo);

        canvas.clear(CanvasKit.Color(250, 250, 250));
        canvas.drawRect(CanvasKit.LTRBRect(10, 10, wrapTo+10, wrapTo+10), paint);
        canvas.drawParagraph(paragraph, 10, 10);

        paint.delete();
        paragraph.delete();
        builder.delete();
        fontMgr.delete();
    });

    it('should not crash if we omit font family on pushed textStyle', () => {
        const surface = CanvasKit.MakeCanvasSurface('test');
        expect(surface).toBeTruthy('Could not make surface');

        const canvas = surface.getCanvas();
        const paint = new CanvasKit.SkPaint();

        paint.setColor(CanvasKit.RED);
        paint.setStyle(CanvasKit.PaintStyle.Stroke);

        const fontMgr = CanvasKit.SkFontMgr.FromData(notoSerifFontBuffer, notoSerifBoldItalicFontBuffer);

        const wrapTo = 250;

        const paraStyle = new CanvasKit.ParagraphStyle({
            textStyle: {
                fontFamilies: ['Noto Serif'],
                fontSize: 20,
            },
            textDirection: CanvasKit.TextDirection.RTL,
            disableHinting: true,
        });

        const builder = CanvasKit.ParagraphBuilder.Make(paraStyle, fontMgr);
        builder.addText('Default text\n');

        const boldItalic = new CanvasKit.TextStyle({
            fontStyle: {
                weight: CanvasKit.FontWeight.Bold,
                slant: CanvasKit.FontSlant.Italic,
            }
        });
        builder.pushStyle(boldItalic);
        builder.addText(`Bold, Italic\n`); // doesn't show up, but we don't crash
        builder.pop();
        builder.addText(`back to normal`);
        const paragraph = builder.build();

        paragraph.layout(wrapTo);

        canvas.clear(CanvasKit.Color(250, 250, 250));
        canvas.drawRect(CanvasKit.LTRBRect(10, 10, wrapTo+10, wrapTo+10), paint);
        canvas.drawParagraph(paragraph, 10, 10);

        surface.flush();

        paragraph.delete();
        builder.delete();
        paint.delete();
        fontMgr.delete();
    });

    it('should not crash if we omit font family on paragraph style', () => {
        const surface = CanvasKit.MakeCanvasSurface('test');
        expect(surface).toBeTruthy('Could not make surface');

        const canvas = surface.getCanvas();
        const paint = new CanvasKit.SkPaint();

        paint.setColor(CanvasKit.RED);
        paint.setStyle(CanvasKit.PaintStyle.Stroke);

        const fontMgr = CanvasKit.SkFontMgr.FromData(notoSerifFontBuffer, notoSerifBoldItalicFontBuffer);

        const wrapTo = 250;

        const paraStyle = new CanvasKit.ParagraphStyle({
            textStyle: {
                fontSize: 20,
            },
            textDirection: CanvasKit.TextDirection.RTL,
            disableHinting: true,
        });

        const builder = CanvasKit.ParagraphBuilder.Make(paraStyle, fontMgr);
        builder.addText('Default text\n');

        const boldItalic = new CanvasKit.TextStyle({
            fontStyle: {
                weight: CanvasKit.FontWeight.Bold,
                slant: CanvasKit.FontSlant.Italic,
            }
        });
        builder.pushStyle(boldItalic);
        builder.addText(`Bold, Italic\n`);
        builder.pop();
        builder.addText(`back to normal`);
        const paragraph = builder.build();

        paragraph.layout(wrapTo);

        canvas.clear(CanvasKit.Color(250, 250, 250));
        canvas.drawRect(CanvasKit.LTRBRect(10, 10, wrapTo+10, wrapTo+10), paint);
        canvas.drawParagraph(paragraph, 10, 10);

        surface.flush();

        paragraph.delete();
        paint.delete();
        fontMgr.delete();
        builder.delete();
    });
});

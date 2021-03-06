#ifndef HGMARKDOWNHIGHLIGHTER_H
#define HGMARKDOWNHIGHLIGHTER_H

#include <QTextCharFormat>
#include <QSyntaxHighlighter>
#include <QAtomicInt>
#include <QSet>
#include <QList>
#include <QString>
#include <QHash>

extern "C" {
#include <pmh_parser.h>
}

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

struct HighlightingStyle
{
    pmh_element_type type;
    QTextCharFormat format;
};

enum HighlightBlockState
{
    Normal = 0,

    // A fenced code block.
    CodeBlock,

    // This block is inside a HTML comment region.
    Comment
};

// One continuous region for a certain markdown highlight style
// within a QTextBlock.
// Pay attention to the change of HighlightingStyles[]
struct HLUnit
{
    // Highlight offset @start and @length with style HighlightingStyles[styleIndex]
    // within a QTextBlock
    unsigned long start;
    unsigned long length;
    unsigned int styleIndex;
};

struct HLUnitStyle
{
    unsigned long start;
    unsigned long length;
    QString style;
};

// Fenced code block only.
struct VCodeBlock
{
    int m_startPos;
    int m_startBlock;
    int m_endBlock;
    QString m_lang;

    QString m_text;
};

// Highlight unit with global position and string style name.
struct HLUnitPos
{
    HLUnitPos() : m_position(-1), m_length(-1)
    {
    }

    HLUnitPos(int p_position, int p_length, const QString &p_style)
        : m_position(p_position), m_length(p_length), m_style(p_style)
    {
    }

    int m_position;
    int m_length;
    QString m_style;
};

// HTML comment.
struct VCommentRegion
{
    VCommentRegion() : m_startPos(0), m_endPos(0) {}

    VCommentRegion(int p_start, int p_end) : m_startPos(p_start), m_endPos(p_end) {}

    // The start position of the region in document.
    int m_startPos;

    // The end position of the region in document.
    int m_endPos;

    // Whether this region contains @p_pos.
    bool contains(int p_pos) const
    {
        return m_startPos <= p_pos && m_endPos >= p_pos;
    }
};

class HGMarkdownHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    HGMarkdownHighlighter(const QVector<HighlightingStyle> &styles,
                          const QHash<QString, QTextCharFormat> &codeBlockStyles,
                          int waitInterval,
                          QTextDocument *parent = 0);
    ~HGMarkdownHighlighter();
    // Request to update highlihgt (re-parse and re-highlight)
    void setCodeBlockHighlights(const QList<HLUnitPos> &p_units);

signals:
    void highlightCompleted();
    void codeBlocksUpdated(const QList<VCodeBlock> &p_codeBlocks);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

public slots:
    void updateHighlight();

private slots:
    void handleContentChange(int position, int charsRemoved, int charsAdded);
    void timerTimeout();

private:
    QRegExp codeBlockStartExp;
    QRegExp codeBlockEndExp;
    QTextCharFormat codeBlockFormat;
    QTextCharFormat m_linkFormat;
    QTextCharFormat m_imageFormat;

    QTextDocument *document;
    QVector<HighlightingStyle> highlightingStyles;
    QHash<QString, QTextCharFormat> m_codeBlockStyles;
    QVector<QVector<HLUnit> > blockHighlights;

    // Use another member to store the codeblocks highlights, because the highlight
    // sequence is blockHighlights, regular-expression-based highlihgts, and then
    // codeBlockHighlights.
    // Support fenced code block only.
    QVector<QVector<HLUnitStyle> > m_codeBlockHighlights;

    int m_numOfCodeBlockHighlightsToRecv;

    // All HTML comment regions.
    QVector<VCommentRegion> m_commentRegions;

    // Timer to signal highlightCompleted().
    QTimer *m_completeTimer;

    QAtomicInt parsing;
    QTimer *timer;
    int waitInterval;

    char *content;
    int capacity;
    pmh_element **result;

    static const int initCapacity;

    void resizeBuffer(int newCap);
    void highlightCodeBlock(const QString &text);
    void highlightLinkWithSpacesInURL(const QString &p_text);
    void parse();
    void parseInternal();
    void initBlockHighlightFromResult(int nrBlocks);
    void initBlockHighlihgtOne(unsigned long pos, unsigned long end,
                               int styleIndex);

    // Return true if there are fenced code blocks and it will call rehighlight() later.
    // Return false if there is none.
    bool updateCodeBlocks();

    // Fetch all the HTML comment regions from parsing result.
    void initHtmlCommentRegionsFromResult();

    // Whether @p_block is totally inside a HTML comment.
    bool isBlockInsideCommentRegion(const QTextBlock &p_block) const;

    // Highlights have been changed. Try to signal highlightCompleted().
    void highlightChanged();
};

#endif

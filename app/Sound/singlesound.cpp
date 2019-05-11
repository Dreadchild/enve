#include "singlesound.h"
#include "soundcomposition.h"
#include "durationrectangle.h"
#include "filesourcescache.h"
#include "Decode/audiodecode.h"
#include "GUI/BoxesList/boxscrollwidgetvisiblepart.h"

SingleSound::SingleSound(const qsptr<FixedLenAnimationRect>& durRect) :
    ComplexAnimator("sound") {
    mOwnDurationRectangle = !durRect;
    if(mOwnDurationRectangle) {
        mDurationRectangle = SPtrCreate(FixedLenAnimationRect)(this);
        mDurationRectangle->setBindToAnimationFrameRange();
        connect(mDurationRectangle.get(), &DurationRectangle::posChanged,
                this, &SingleSound::anim_updateAfterShifted);
    } else mDurationRectangle = durRect;

    connect(mDurationRectangle.get(), &DurationRectangle::rangeChanged,
            this, &SingleSound::prp_afterWholeInfluenceRangeChanged);
    connect(mDurationRectangle.get(), &DurationRectangle::posChanged,
            this, &SingleSound::updateAfterDurationRectangleShifted);
    mDurationRectangle->setSoundCacheHandler(getCacheHandler());

    ca_addChildAnimator(mVolumeAnimator);
}

DurationRectangleMovable *SingleSound::anim_getTimelineMovable(
        const int &relX, const int &minViewedFrame,
        const qreal &pixelsPerFrame) {
    if(!mDurationRectangle) return nullptr;
    return mDurationRectangle->getMovableAt(relX, pixelsPerFrame,
                                            minViewedFrame);
}

void SingleSound::drawTimelineControls(QPainter * const p,
                                       const qreal &pixelsPerFrame,
                                       const FrameRange &absFrameRange,
                                       const int &rowHeight) {
//    qreal timeScale = mTimeScaleAnimator.getCurrentValue();
//    int startDFrame = mDurationRectangle.getMinAnimationFrame() - startFrame;
//    int frameWidth = ceil(mListOfFrames.count()/qAbs(timeScale));
//    p->fillRect(startDFrame*pixelsPerFrame + pixelsPerFrame*0.5, drawY,
//                frameWidth*pixelsPerFrame - pixelsPerFrame,
//                BOX_HEIGHT, QColor(0, 0, 255, 125));
    if(mDurationRectangle) {
        p->save();
        if(mOwnDurationRectangle)
            p->translate(prp_getParentFrameShift()*pixelsPerFrame, 0);
        const int width = qFloor(absFrameRange.span()*pixelsPerFrame);
        const QRect drawRect(0, 0, width, rowHeight);
        mDurationRectangle->draw(p, drawRect, getCanvasFPS(),
                                 pixelsPerFrame, absFrameRange);
        p->restore();
    }
    ComplexAnimator::drawTimelineControls(p, pixelsPerFrame,
                                          absFrameRange, rowHeight);
}

FixedLenAnimationRect *SingleSound::getDurationRect() const {
    return mDurationRectangle.get();
}

#include "canvas.h"

int SingleSound::getSampleShift() const{
    const qreal fps = getCanvasFPS();
    return qRound(prp_getFrameShift()*(SOUND_SAMPLERATE/fps));
}

SampleRange SingleSound::relSampleRange() const {
    const qreal fps = getCanvasFPS();
    const auto durRect = getDurationRect();
    const auto relFrameRange = durRect->getRelFrameRange();
    const auto qRelFrameRange = qValueRange{qreal(relFrameRange.fMin),
                                            qreal(relFrameRange.fMax)};
    const auto qSampleRange = qRelFrameRange*(SOUND_SAMPLERATE/fps);
    return {qFloor(qSampleRange.fMin), qCeil(qSampleRange.fMax)};
}

SampleRange SingleSound::absSampleRange() const {
    const qreal fps = getCanvasFPS();
    const auto durRect = getDurationRect();
    const auto absFrameRange = durRect->getAbsFrameRange();
    const auto qAbsFrameRange = qValueRange{qreal(absFrameRange.fMin),
                                            qreal(absFrameRange.fMax)};
    const auto qSampleRange = qAbsFrameRange*(SOUND_SAMPLERATE/fps);
    return {qFloor(qSampleRange.fMin), qCeil(qSampleRange.fMax)};
}

iValueRange SingleSound::absSecondToRelSeconds(const int &absSecond) {
    const qreal fps = getCanvasFPS();
    const qreal speed = isZero6Dec(mStretch) ? TEN_MIL : 1/mStretch;
    const qreal qFirstSecond = prp_absFrameToRelFrameF(absSecond*fps)*speed/fps;
    if(isInteger4Dec(qFirstSecond)) {
        const int round = qRound(qFirstSecond);
        if(isOne4Dec(mStretch) || mStretch > 1) {
            return {round, round};
        }
        const qreal qLast = qFirstSecond + speed - 1;
        if(isInteger4Dec(qLast)) {
            const int roundLast = qRound(qLast);
            return {round, roundLast};
        }
        const int ceilLast = qMax(round, qCeil(qLast));
        return {round, ceilLast};
    }
    const int firstSecond = qFloor(qFirstSecond);
    const int lastSecond = qCeil(qFirstSecond + speed - 1);
    return {firstSecond, lastSecond};
}

void SingleSound::setStretch(const qreal &stretch) {
    mStretch = stretch;
    updateDurationRectLength();
}

void SingleSound::updateDurationRectLength() {
    if(mOwnDurationRectangle) {
        const int secs = mCacheHandler ? mCacheHandler->durationSec() : 0;
        const qreal fps = getCanvasFPS();
        const int frames = qCeil(qAbs(secs*fps*mStretch));
        mDurationRectangle->setAnimationFrameDuration(frames);
    }
}

qreal SingleSound::getCanvasFPS() const {
    auto parentCanvas = getFirstAncestor<Canvas>();
    if(!parentCanvas) {
        const auto box = getFirstAncestor<BoundingBox>();
        if(!box) return 1;
        parentCanvas = box->getParentCanvas();
        if(!parentCanvas) return 1;
    }
    return parentCanvas->getFps();
}

void SingleSound::updateAfterDurationRectangleShifted() {
    prp_afterFrameShiftChanged();
    anim_setAbsFrame(anim_getCurrentAbsFrame());
    prp_afterWholeInfluenceRangeChanged();
}

void SingleSound::setFilePath(const QString &path) {
    if(path.isEmpty()) mCacheHandler.clear();
    else mCacheHandler = FileSourcesCache::getHandlerForFilePath
                <SoundCacheHandler>(path);
    mDurationRectangle->setSoundCacheHandler(getCacheHandler());
    updateDurationRectLength();
}

int SingleSound::prp_getRelFrameShift() const {
    if(mOwnDurationRectangle)
        return mDurationRectangle->getFrameShift();
    return 0;
}

bool SingleSound::SWT_shouldBeVisible(const SWT_RulesCollection &rules,
                                      const bool &parentSatisfies,
                                      const bool &parentMainTarget) const {
    if(rules.fType == SWT_TYPE_SOUND) return true;
    if(rules.fType == SWT_TYPE_GRAPHICS) return false;
    return SingleWidgetTarget::SWT_shouldBeVisible(rules,
                                                   parentSatisfies,
                                                   parentMainTarget);
}

FrameRange SingleSound::prp_relInfluenceRange() const {
    return mDurationRectangle->getAbsFrameRange();
}

#include "basicreadwrite.h"
void SingleSound::writeProperty(QIODevice * const target) const {
    const auto filePath = mCacheHandler ? mCacheHandler->getFilePath() : "";
    gWrite(target, filePath);
    mVolumeAnimator->writeProperty(target);
    mDurationRectangle->writeDurationRectangle(target);
}

void SingleSound::readProperty(QIODevice * const target) {
    const QString filePath = gReadString(target);
    mVolumeAnimator->readProperty(target);
    mDurationRectangle->readDurationRectangle(target);
}

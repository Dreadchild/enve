#ifndef PATHBOX_H
#define PATHBOX_H
#include "Boxes/boundingbox.h"
#include "gradientpoints.h"
#include "Animators/paintsettings.h"

class PathBox : public BoundingBox
{
public:
    PathBox(BoxesGroup *parent,
            const BoundingBoxType &type);
    ~PathBox();

    void draw(QPainter *p);

    void schedulePathUpdate();

    void updatePathIfNeeded();

    void resetStrokeGradientPointsPos(bool finish) {
        mStrokeGradientPoints->prp_setRecording(false);
        mStrokeGradientPoints->setPositions(mRelBoundingRect.topLeft(),
                                           mRelBoundingRect.bottomRight(),
                                           finish);
    }

    void resetFillGradientPointsPos(bool finish) {
        mFillGradientPoints->prp_setRecording(false);
        mFillGradientPoints->setPositions(mRelBoundingRect.topLeft(),
                                         mRelBoundingRect.bottomRight(),
                                         finish);
    }

    virtual void setStrokeCapStyle(Qt::PenCapStyle capStyle) {
        mStrokeSettings->setCapStyle(capStyle);
        clearAllCache();
        scheduleOutlinePathUpdate();
    }

    virtual void setStrokeJoinStyle(Qt::PenJoinStyle joinStyle) {
        mStrokeSettings->setJoinStyle(joinStyle);
        clearAllCache();
        scheduleOutlinePathUpdate();
    }

    virtual void setStrokeWidth(qreal strokeWidth, bool finish) {
        mStrokeSettings->setCurrentStrokeWidth(strokeWidth);
        if(finish) {
            mStrokeSettings->getStrokeWidthAnimator()->prp_finishTransform();
        }
        //scheduleOutlinePathUpdate();
    }

    void setOutlineCompositionMode(QPainter::CompositionMode compositionMode) {
        mStrokeSettings->setOutlineCompositionMode(compositionMode);
        clearAllCache();
        scheduleSoftUpdate();
    }

    void startSelectedStrokeWidthTransform() {
        mStrokeSettings->getStrokeWidthAnimator()->prp_startTransform();
    }

    void startSelectedStrokeColorTransform() {
        mStrokeSettings->getColorAnimator()->prp_startTransform();
    }

    void startSelectedFillColorTransform() {
        mFillSettings->getColorAnimator()->prp_startTransform();
    }

    StrokeSettings *getStrokeSettings();
    PaintSettings *getFillSettings();
    void updateDrawGradients();

    void updateOutlinePath();
    void scheduleOutlinePathUpdate();
    void updateOutlinePathIfNeeded();

    void setOutlineAffectedByScale(bool bT);
    int prp_saveToSql(QSqlQuery *query, const int &parentId);
    void prp_loadFromSql(const int &boundingBoxId);
    void updateRelBoundingRect();

    void setUpdateVars();

    VectorPath *objectToPath();
    const QPainterPath &getRelativePath() const { return mPath; }
    bool relPointInsidePath(QPointF relPos);
    void preUpdatePixmapsUpdates();

    void duplicateGradientPointsFrom(GradientPoints *fillGradientPoints,
                                     GradientPoints *strokeGradientPoints);
    void duplicatePaintSettingsFrom(PaintSettings *fillSettings,
                                    StrokeSettings *strokeSettings);

    void prp_makeDuplicate(Property *targetBox);

    virtual void drawHovered(QPainter *p) {
        drawHoveredPath(p, mPath);
    }

    void applyPaintSetting(
            const PaintSetting &setting) {
        setting.apply(this);
        scheduleSoftUpdate();
    }

    void setFillColorMode(const ColorMode &colorMode) {
        mFillSettings->getColorAnimator()->setColorMode(colorMode);
    }
    void setStrokeColorMode(const ColorMode &colorMode) {
        mFillSettings->getColorAnimator()->setColorMode(colorMode);
    }
    void updateStrokeDrawGradient();
    void updateFillDrawGradient();

    virtual void updatePath() {}
protected:
    QSharedPointer<GradientPoints> mFillGradientPoints =
            (new GradientPoints)->ref<GradientPoints>();
    QSharedPointer<GradientPoints> mStrokeGradientPoints =
            (new GradientPoints)->ref<GradientPoints>();


    QLinearGradient mDrawFillGradient;
    QLinearGradient mDrawStrokeGradient;

    QSharedPointer<PaintSettings> mFillSettings =
            (new PaintSettings)->ref<PaintSettings>();
    QSharedPointer<StrokeSettings> mStrokeSettings =
            (new StrokeSettings)->ref<StrokeSettings>();


    bool mPathUpdateNeeded = false;
    bool mOutlinePathUpdateNeeded = false;

    QPainterPath mUpdatePath;
    QPainterPath mUpdateOutlinePath;
    bool mFillSettingsGradientUpdateNeeded = false;
    bool mStrokeSettingsGradientUpdateNeeded = false;
    UpdatePaintSettings mUpdateFillSettings;
    UpdateStrokeSettings mUpdateStrokeSettings;

    QPainterPath mPath;
    QPainterPath mOutlinePath;
    QPainterPath mWholePath;
    void updateWholePath();

    bool mOutlineAffectedByScale = true;
};

#endif // PATHBOX_H

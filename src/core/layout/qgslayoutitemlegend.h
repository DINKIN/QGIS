/***************************************************************************
                         qgslayoutitemlegend.h
                         ---------------------
    begin                : October 2017
    copyright            : (C) 2017 by Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSLAYOUTITEMLEGEND_H
#define QGSLAYOUTITEMLEGEND_H

#include "qgis_core.h"
#include "qgis.h"
#include "qgslayoutitem.h"
#include "qgslayertreemodel.h"
#include "qgslegendsettings.h"
#include "qgslayertreegroup.h"

class QgsLayerTreeModel;
class QgsSymbol;
class QgsLayoutItemMap;
class QgsLegendRenderer;

/**
 * \ingroup core
 * Item model implementation based on layer tree model for layout legend.
 *
 * Overrides some functionality of QgsLayerTreeModel to better fit the needs of layout legends.
 *
 * \since QGIS 2.6
 */
class CORE_EXPORT QgsLegendModel : public QgsLayerTreeModel
{
    Q_OBJECT

  public:
    //! Construct the model based on the given layer tree
    QgsLegendModel( QgsLayerTree *rootNode, QObject *parent SIP_TRANSFERTHIS = 0 );

    QVariant data( const QModelIndex &index, int role ) const override;

    Qt::ItemFlags flags( const QModelIndex &index ) const override;
};



/**
 * \ingroup core
 * A layout item subclass for map legends.
 * \since QGIS 3.0
 */
class CORE_EXPORT QgsLayoutItemLegend : public QgsLayoutItem
{
    Q_OBJECT

  public:

    /**
     * Constructor for QgsLayoutItemLegend, with the specified parent \a layout.
     */
    QgsLayoutItemLegend( QgsLayout *layout );

    /**
     * Returns a new legend item for the specified \a layout.
     *
     * The caller takes responsibility for deleting the returned object.
     */
    static QgsLayoutItemLegend *create( QgsLayout *layout ) SIP_FACTORY;

    int type() const override;
    QString stringType() const override;
    //Overridden to show legend title
    QString displayName() const override;

    /**
     * Sets the legend's item bounds to fit the whole legend content.
     */
    void adjustBoxSize();

    /**
     * Sets whether the legend should automatically resize to fit its contents.
     * \param enabled set to false to disable automatic resizing. The legend frame will not
     * be expanded to fit legend items, and items may be cropped from display.
     * \see resizeToContents()
     */
    void setResizeToContents( bool enabled );

    /**
     * Returns whether the legend should automatically resize to fit its contents.
     * \see setResizeToContents()
     */
    bool resizeToContents() const;

    /**
     * Returns the legend model.
     */
    QgsLegendModel *model() { return mLegendModel.get(); }

    /**
     * Sets whether the legend content should auto update to reflect changes in the project's
     * layer tree.
     * \see autoUpdateModel()
     */
    void setAutoUpdateModel( bool autoUpdate );

    /**
     * Returns whether the legend content should auto update to reflect changes in the project's
     * layer tree.
     * \see setAutoUpdateModel()
     */
    bool autoUpdateModel() const;

    /**
     * Set whether legend items should be filtered to show just the ones visible in the associated map.
     * \see legendFilterByMapEnabled()
     */
    void setLegendFilterByMapEnabled( bool enabled );

    /**
     * Find out whether legend items are filtered to show just the ones visible in the associated map
     * \see setLegendFilterByMapEnabled()
     */
    bool legendFilterByMapEnabled() const { return mLegendFilterByMap; }

#if 0//TODO

    /**
     * Update() overloading. Use it rather than update()
     */
    virtual void updateItem() override;
#endif

    /**
     * When set to true, during an atlas rendering, it will filter out legend elements
     * where features are outside the current atlas feature.
     * \see legendFilterOutAtlas()
     */
    void setLegendFilterOutAtlas( bool doFilter );

    /**
     * Returns whether to filter out legend elements outside of the current atlas feature.
     * \see setLegendFilterOutAtlas()
     */
    bool legendFilterOutAtlas() const;

    /**
     * Sets the legend \a title.
     * \see title()
     */
    void setTitle( const QString &title );

    /**
     * Returns the legend title.
     * \see setTitle()
     */
    QString title() const;

    /**
     * Returns the alignment of the legend title.
     * \see setTitleAlignment()
     */
    Qt::AlignmentFlag titleAlignment() const;

    /**
     * Sets the \a alignment of the legend title.
     * \see titleAlignment()
     */
    void setTitleAlignment( Qt::AlignmentFlag alignment );

    /**
     * Returns reference to modifiable legend style.
     */
    QgsLegendStyle &rstyle( QgsLegendStyle::Style s );

    /**
     * Returns legend style.
     */
    QgsLegendStyle style( QgsLegendStyle::Style s ) const;

    /**
     * Sets the style of \a component to \a style for the legend.
     */
    void setStyle( QgsLegendStyle::Style component, const QgsLegendStyle &style );

    /**
     * Returns the font settings for a legend \a component.
     * \see setStyleFont()
     */
    QFont styleFont( QgsLegendStyle::Style component ) const;

    /**
     * Sets the style \a font for a legend \a component.
     * \see styleFont()
     */
    void setStyleFont( QgsLegendStyle::Style component, const QFont &font );

    /**
     * Set the \a margin for a legend \a component.
     */
    void setStyleMargin( QgsLegendStyle::Style component, double margin );

    /**
     * Set the \a margin for a particular \a side of a legend \a component.
     */
    void setStyleMargin( QgsLegendStyle::Style component, QgsLegendStyle::Side side, double margin );

    /**
     * Returns the spacing in-between lines in layout units.
     * \see setLineSpacing()
     */
    double lineSpacing() const;

    /**
     * Sets the \a spacing in-between multiple lines.
     * \see lineSpacing()
     */
    void setLineSpacing( double spacing );

    /**
     * Returns the legend box space.
     * \see setBoxSpace()
     */
    double boxSpace() const;

    /**
     * Sets the legend box \a space.
     * \see boxSpace()
     */
    void setBoxSpace( double space );

    /**
     * Returns the legend column spacing.
     * \see setColumnSpace()
     */
    double columnSpace() const;

    /**
     * Sets the legend column \a spacing.
     * \see columnSpace()
     */
    void setColumnSpace( double spacing );

    /**
     * Returns the legend font color.
     * \see setFontColor()
     */
    QColor fontColor() const;

    /**
     * Sets the legend font \a color.
     * \see fontColor()
     */
    void setFontColor( const QColor &color );

    /**
     * Returns the legend symbol width.
     * \see setSymbolWidth()
     */
    double symbolWidth() const;

    /**
     * Sets the legend symbol \a width.
     * \see symbolWidth()
     */
    void setSymbolWidth( double width );

    /**
     * Returns the legend symbol height.
     * \see setSymbolHeight()
     */
    double symbolHeight() const;

    /**
     * Sets the legend symbol \a height.
     * \see symbolHeight()
     */
    void setSymbolHeight( double height );

    /**
     * Returns the WMS legend width.
     * \see setWmsLegendWidth()
     */
    double wmsLegendWidth() const;

    /**
     * Sets the WMS legend \a width.
     * \see wmsLegendWidth()
     */
    void setWmsLegendWidth( double width );

    /**
     * Returns the WMS legend height.
     * \see setWmsLegendHeight()
     */
    double wmsLegendHeight() const;

    /**
     * Sets the WMS legend \a height.
     * \see wmsLegendHeight()
     */
    void setWmsLegendHeight( double height );

    /**
     * Sets the legend text wrapping \a string.
     * \see wrapString()
     */
    void setWrapString( const QString &string );

    /**
     * Returns the legend text wrapping string.
     * \see setWrapString()
     */
    QString wrapString() const;

    /**
     * Returns the legend column count.
     * \see setColumnCount()
     */
    int columnCount() const;

    /**
     * Sets the legend column \a count.
     * \see columnCount()
     */
    void setColumnCount( int count );

    /**
     * Returns whether the legend items from a single layer can be split
     * over multiple columns.
     * \see setSplitLayer()
     */
    bool splitLayer() const;

    /**
     * Sets whether the legend items from a single layer can be split
     * over multiple columns.
     * \see splitLayer()
     */
    void setSplitLayer( bool enabled );

    /**
     * Returns whether column widths should be equalized.
     * \see setEqualColumnWidth()
     */
    bool equalColumnWidth() const;

    /**
     * Sets whether column widths should be equalized.
     * \see equalColumnWidth()
     */
    void setEqualColumnWidth( bool equalize );

    /**
     * Returns whether a stroke will be drawn around raster symbol items.
     * \see setDrawRasterStroke()
     * \see rasterStrokeColor()
     * \see rasterStrokeWidth()
     */
    bool drawRasterStroke() const;

    /**
     * Sets whether a stroke will be drawn around raster symbol items.
     * \param enabled set to true to draw borders
     * \see drawRasterStroke()
     * \see setRasterStrokeColor()
     * \see setRasterStrokeWidth()
     */
    void setDrawRasterStroke( bool enabled );

    /**
     * Returns the stroke color for the stroke drawn around raster symbol items. The stroke is
     * only drawn if drawRasterStroke() is true.
     * \see setRasterStrokeColor()
     * \see drawRasterStroke()
     * \see rasterStrokeWidth()
     */
    QColor rasterStrokeColor() const;

    /**
     * Sets the stroke \a color for the stroke drawn around raster symbol items. The stroke is
     * only drawn if drawRasterStroke() is true.
     * \see rasterStrokeColor()
     * \see setDrawRasterStroke()
     * \see setRasterStrokeWidth()
     */
    void setRasterStrokeColor( const QColor &color );

    /**
     * Returns the stroke width (in layout units) for the stroke drawn around raster symbol items. The stroke is
     * only drawn if drawRasterStroke() is true.
     * \see setRasterStrokeWidth()
     * \see drawRasterStroke()
     * \see rasterStrokeColor()
     */
    double rasterStrokeWidth() const;

    /**
     * Sets the stroke width for the stroke drawn around raster symbol items. The stroke is
     * only drawn if drawRasterStroke() is true.
     * \see rasterStrokeWidth()
     * \see setDrawRasterStroke()
     * \see setRasterStrokeColor()
     */
    void setRasterStrokeWidth( double width );

    /**
     * Sets the \a map to associate with the legend.
     * \see map()
     */
    void setMap( QgsLayoutItemMap *map );

    /**
     * Returns the associated map.
     * \see setMap()
     */
    QgsLayoutItemMap *map() const { return mMap; }

    /**
     * Updates the model and all legend entries.
     */
    void updateLegend();

    /**
     * Updates the legend content when filtered by map.
     */
    void updateFilterByMap( bool redraw = true );

    /**
     * Returns the legend's renderer settings object.
     */
    const QgsLegendSettings &legendSettings() const { return mSettings; }

    void paint( QPainter *painter, const QStyleOptionGraphicsItem *itemStyle, QWidget *pWidget ) override;


  public slots:

    void refreshDataDefinedProperty( const QgsLayoutObject::DataDefinedProperty property = QgsLayoutObject::AllProperties ) override;

  protected:
    void draw( QgsRenderContext &context, const QStyleOptionGraphicsItem *itemStyle = nullptr ) override;
    bool writePropertiesToElement( QDomElement &element, QDomDocument &document, const QgsReadWriteContext &context ) const override;
    bool readPropertiesFromElement( const QDomElement &element, const QDomDocument &document, const QgsReadWriteContext &context ) override;

  private slots:

    //! Data changed
    void synchronizeWithModel();

    //! Removes the associated map if the map is deleted.
    void invalidateCurrentMap();

    void updateFilterByMapAndRedraw();


    //! update legend in case style of associated map has changed
    void mapLayerStyleOverridesChanged();

    //! react to atlas
    void onAtlasEnded();
    void onAtlasFeature( QgsFeature * );

    void nodeCustomPropertyChanged( QgsLayerTreeNode *node, const QString &key );

  private:
    QgsLayoutItemLegend() = delete;

    //! use new custom layer tree and update model. if new root is null pointer, will use project's tree
    void setCustomLayerTree( QgsLayerTree *rootGroup );

    std::unique_ptr< QgsLegendModel > mLegendModel;
    std::unique_ptr< QgsLayerTreeGroup > mCustomLayerTree;

    QgsLegendSettings mSettings;

    QString mTitle;
    int mColumnCount = 1;

    QgsLayoutItemMap *mMap = nullptr;

    bool mLegendFilterByMap = false;
    bool mLegendFilterByExpression = false;

    //! whether to filter out legend elements outside of the atlas feature
    bool mFilterOutAtlas = false;

    //! tag for update request
    bool mFilterAskedForUpdate = false;
    //! actual filter update
    void doUpdateFilterByMap();

    bool mInAtlas = false;

    //! Will be false until the associated map scale and DPI have been calculated
    bool mInitialMapScaleCalculated = false;

    //! Will be true if the legend size should be totally reset at next paint
    bool mForceResize = false;

    //! Will be true if the legend should be resized automatically to fit contents
    bool mSizeToContents = true;
};

#endif // QGSLAYOUTITEMLEGEND_H


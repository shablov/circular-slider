<ol>
<blockquote><li>Changes colors</li>
<ul>
<blockquote><li>Background brush (default is white; setBackgroundBrush(const QBrush&));</li>
<li>Circle brush (setCircleBrush(const QBrush&));</li>
<li>Sectors brush (setSectorsBrush(const QBrush&));</li>
<li>Sectors pen (setSectorsPen(const QPen&));</li>
<li>Arrow pen (setArrowPen(const QPen&));</li>
<li>Scale pen (setScalePen(const QPen&));</li>
</blockquote></ul></blockquote>

<blockquote><li>Сhanges the characteristics of the scale</li>
<ul>
<blockquote><li>Offset (default offset is 0 and 0&deg at the top; setOffset(const double&))</li>
<li>Scale direction (default counterclockwise; setDirectiorn(bool clockwise)</li>
<li>All parameters (setScaleParameters(const double &offset, bool clockwise)</li>
</blockquote></ul></blockquote>

<blockquote><li>Addition tools for control</li>
<ul>
<blockquote><li>Clear sectors list (clearSectors())</li>
<li>Add sector (addSector(const QPair<double, double> &)</li>
</blockquote></ul></blockquote>

<blockquote><li>Work with angle</li>
<ul>
<blockquote><li>Set angle, scene repaint with this angle (setAngle(const double&))</li>
<li>Get angle (angle())</li>
</blockquote></ul></blockquote>

</ol>
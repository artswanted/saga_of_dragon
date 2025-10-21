using System;
using System.ComponentModel;
using System.ComponentModel.Design.Serialization;

namespace SegmentedTrackBarDLL
{
    [ToolboxItem(false)]
    [DesignTimeVisible(false)]
    [TypeConverter(typeof(SegmentedTrackBarDLL.CollectionItemConverter))]
    public abstract class CollectionItem: System.ComponentModel.Component, IDisposable
    {
        internal Collection collection;

        ~CollectionItem()
        {
            Dispose(false);
        }

        [Browsable(false)]
        public Collection Collection
        {
            get {return collection;}
        }
    }
}

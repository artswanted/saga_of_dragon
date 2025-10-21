using System;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;
using System.ComponentModel.Design;

namespace SegmentedTrackBarDLL
{
	/// <summary>
	/// Summary description for Track.
	/// </summary>
	[TypeConverter(typeof(ExpandableObjectConverter))]
	public class Track
	{
        private Color m_InRangeColor;
        private Color m_OutOfRangeColor;

        public Color InRangeColor
        {
            get
            {
                return m_InRangeColor;
            }
            set
            {
                m_InRangeColor = value;
            }
        }

        public Color OutOfRangeColor
        {
            get
            {
                return m_OutOfRangeColor;
            }
            set
            {
                m_OutOfRangeColor = value;
            }
        }
	}
}

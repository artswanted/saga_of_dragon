using System;
using System.ComponentModel;
using System.ComponentModel.Design.Serialization;

namespace SegmentedTrackBarDLL
{
    internal class CollectionItemConverter: TypeConverter
    {
        public override Boolean CanConvertTo(ITypeDescriptorContext context, Type destinationType)
        {
            if (destinationType == typeof(InstanceDescriptor))
                return true;
            return base.CanConvertTo(context, destinationType);
        }

        public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType)
        {
            if (destinationType == typeof(InstanceDescriptor))
            {
                System.Reflection.ConstructorInfo ci = value.GetType().GetConstructor(System.Type.EmptyTypes);
                return new InstanceDescriptor(ci, null, false);
            }
            return base.ConvertTo(context, culture, value, destinationType);
        }
    }
}

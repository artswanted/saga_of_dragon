using System;
using System.Reflection;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
	/// <summary>
	/// Summary description for EventChecker.
	/// </summary>
	public class EventChecker
	{
		public EventChecker()
		{
		}

		public static void CheckForRegisteredEvents(object obj)
		{
			Type type = obj.GetType();
			FieldInfo[] fieldInfo = type.GetFields(BindingFlags.Instance |
				BindingFlags.Public | BindingFlags.NonPublic );
			Type delegateType = typeof(System.Delegate);
			foreach(FieldInfo field in fieldInfo)
			{
				if (delegateType.IsAssignableFrom(field.FieldType))
				{
					Delegate del = field.GetValue(obj) as Delegate;
					if (del != null)
					{
						object target = del.Target;
						if (target != null)
						{
							Console.WriteLine("Registered Event found for :"
								+ obj + "." + field.Name 
								+ Environment.NewLine 
								+ "Target:" + target 
								+ Environment.NewLine
								+ "Method:" + del.Method);
						}
					}
				}
			}
		}
	}
}

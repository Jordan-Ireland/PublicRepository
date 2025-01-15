using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SODatabase<T> : ScriptableObject
{
    [SerializeField] public List<T> database = new List<T>();
    public int Count { get { return database.Count; } }
    public int currentId = 0;


#if UNITY_EDITOR
    public void Swap(T item, int index)
    {
        database[index] = item;
        UnityEditor.EditorUtility.SetDirty(this);
    }

    public void Add(T item)
    {
        database.Add(item);
        currentId++;
        UnityEditor.EditorUtility.SetDirty(this);
    }

    public void Delete(int index)
    {
        database.RemoveAt(index);
        UnityEditor.EditorUtility.SetDirty(this);
    }
#endif
}

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName = "Custom/ItemDatabase")]
[System.Serializable]
public class ItemDatabase : SODatabase<Item>
{

    [SerializeField] public List<ItemProperty> itemProperties = new List<ItemProperty>();

    public int GetIDByIndex(int index)
    {
        if (index < Count)
            return database[index].Id;
        else 
            return -1;
    }

    public int GetIndexByID(int id)
    {
        for (int i = 0; i < Count; i++)
        {
            if (database[i].Id == id)
            {
                return i;
            }
        }
        return -1;
    }

    public Item GetByID(int id)
    {
        for (int i = 0; i < Count; i++)
        {
            if (database[i].Id == id)
            {
                return database[i];
            }
        }
        return new Item();
    }
    

    public Item GetByIndex(int index)
    {
        return database[index];
    }
}

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName = "Databases/RecipeDatabase")]
[System.Serializable]
public class RecipeDatabase : SODatabase<RecipeBook>
{
    public RecipeBook GetById(int id)
    {
        for (int i = 0; i < Count; i++)
        {
            if (database[i].id == id)
                return database[i];
        }

        return null;
    }

    public RecipeBook GetByIndex(int index)
    {
        return database[index];
    }
}

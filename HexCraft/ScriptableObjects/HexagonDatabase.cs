using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName = "Custom/HexagonManager")]
[System.Serializable]
public class HexagonDatabase : SODatabase<HexagonType>
{

    public HexagonType GetHexagon(HexagonTypes type)
    {
        for (int i = 0; i < Count; i++)
        {
            if(database[i].hexagonType == type)
            {
                return database[i];
            }
        }
        return null;
    }

    public HexagonType GetById(int id)
    {
        for (int i = 0; i < Count; i++)
        {
            if (database[i].id == id)
                return database[i];
        }

        return null;
    }

    public HexagonType GetByIndex(int index)
    {
        return database[index];
    }
}

[System.Serializable]
public class HexagonType
{
    public int id;
    public HexagonTypes hexagonType;
    public bool isCraftable = true;
    public List<HexagonVariation> hexagonVariations = new List<HexagonVariation>();

    public (HexagonVariation, int) GetRandomHexagon()
    {
        if (hexagonVariations.Count == 1) return (hexagonVariations[0], 0);
        int i = Random.Range(0, hexagonVariations.Count);
        return (hexagonVariations[i], i);

        //if (hexagonPrefab.Length == 1) return (hexagonPrefab[0], 0);
        //int i = Random.Range(0, hexagonPrefab.Length);
        //return (hexagonPrefab[i], i);
    }
}

[System.Serializable]
public class HexagonVariation {
    public int expToGive;
    public List<InventoryItem> resources = new List<InventoryItem>();
    public GameObject hexagonPrefab;
}

[System.Serializable]
public struct ItemCrafting
{
    public int id;
    public int amount;
}

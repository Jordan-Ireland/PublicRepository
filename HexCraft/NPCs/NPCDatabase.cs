using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName = "Custom/NPC Database")]
public class NPCDatabase : ScriptableObject
{
    [SerializeField] public List<GameObject> standardEnemies = new List<GameObject>();
    [SerializeField] public List<GameObject> bossEnemies = new List<GameObject>();

    public GameObject GetRandomEnemy()
    {
        int random = Random.Range(0, standardEnemies.Count);
        return standardEnemies[random];
    }

    public GameObject GetBossEnemy()
    {
        int random = Random.Range(0, bossEnemies.Count);
        return bossEnemies[random];
    }

    public GameObject GetById(int id)
    {
        for (int i = 0; i < standardEnemies.Count; i++)
        {
            if (standardEnemies[i].GetComponent<Enemy>().enemy.id == id)
                return standardEnemies[i];
        }
        for (int i = 0; i < bossEnemies.Count; i++)
        {
            if (bossEnemies[i].GetComponent<Enemy>().enemy.id == id)
                return bossEnemies[i];
        }
        return null;
    }
}

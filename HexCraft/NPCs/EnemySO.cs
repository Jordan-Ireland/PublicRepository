using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName = "Custom/Enemy")]
[System.Serializable]
public class EnemySO : ScriptableObject
{
    public string Name;
    public int id;
    public int level = 1;
    public int baseAttack = 1;
    public int expToGive = 5;
    public int dnaToGive = 5;
    public int maxHealth = 50;
    public float speed = 10;
    public float rotationSpeed = 180;
    public bool isBoss = false;
    public float timeBetweenMovements = 30;
    public float timeBetweenAttacks = 2;

    public int MaxHealth
    {
        get
        {
            return maxHealth + (level * 25);
        }
    }
    public int ExpToGive
    {
        get
        {
            return expToGive + (level * 2);
        }
    }

    public int DnaToGive
    {
        get
        {
            return dnaToGive + (level * 2);
        }
    }
}

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Buff : Interactable
{
    public enum Boosts { Speed, Score, Time }
    public Boosts boost;

    // Update is called once per frame
    private void Update()
    {
        if(player != null)
        {
            switch (boost)
            {
                case Boosts.Speed:
                    player.pm.movementMultiplier = 1.5f;
                    break;
                case Boosts.Score:
                    GameManager.AddScore(player.isPlayer1, 10);
                    break;
                case Boosts.Time:
                    GameManager.AddTime(player.isPlayer1, 20);
                    break;
            }
            Destroy(gameObject);
        }
    }
}

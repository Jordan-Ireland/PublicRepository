using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Water : Hexagon, IInteractable
{
    public float waterSpeed = 1f;
    public float scale = 3;
    public Vector2 movementRange;

    Vector3 startPos;
    Vector3 wantedPos;
    bool moving = false;

    public void Use(float dmg, TOOLTYPE type)
    {
        if(type == TOOLTYPE.Bucket)
        {
            (int, InventoryItem) item = GameManager.playerInstance.inventory.GetById(7);
            if(item.Item2 != null)
            {
                GameManager.playerInstance.inventory.AddItemByIndex(item.Item1, 8, 1);
                UIManager.instance.UpdateHandSlots();
            }
        }
    }

    //-0.75 - 0.25

    // Start is called before the first frame update
    void Start()
    {
        //startPos = transform.position;
        //transform.position = new Vector3(startPos.x, -10, startPos.z);
    }

    // Update is called once per frame
    void Update()
    {
        //if(!moving)
        //{
        //    wantedPos = new Vector3(
        //        startPos.x,
        //        Random.Range(movementRange.x * scale, movementRange.y * scale),
        //        startPos.z
        //        );
        //    moving = true;
        //    //StartCoroutine(LerpToPosition());
        //}
        //else
        //{
        //    transform.position = Vector3.Lerp(transform.position, wantedPos, Time.deltaTime * waterSpeed);
        //    if (Vector3.Distance(transform.position, wantedPos) <= 0.01f)
        //    {
        //        startPos = transform.position;
        //        moving = false;
        //    }
        //}
    }

    IEnumerator LerpToPosition()
    {
        moving = true;
        float duration = Random.Range(2f, 4f);
        float time = 0;
        Vector3 startPosition = transform.position;
        while(time < duration)
        {
            transform.position = Vector3.Lerp(startPosition, wantedPos, time / duration);
            time += Time.deltaTime;
            yield return null;
        }
        transform.position = wantedPos;
        moving = false;
    }
}

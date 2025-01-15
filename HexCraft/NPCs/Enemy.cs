using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class Enemy : MonoBehaviour, IInteractable
{
    public EnemySO enemy;
    CharacterController controller;

    Ray[] rays;
    public float currentHealth;
    public int index;
    public enum AIState { Idle, Moving, Chasing, Attacking }
    public AIState aiState = AIState.Idle;
    float timeSinceMove;
    float timeSinceAttack;
    float timeOffset;
    Vector3 wanted;
    Rigidbody rb;
    Hexagon hex;
    //values for internal use
    private Quaternion _lookRotation;
    private Vector3 _direction;
    Vector3 velocity;

    float stoppingDis;

    bool pushback = false;

    public void Init(int index, Hexagon hex)
    {
        this.index = index;
        this.hex = hex;
        transform.position = GetRandomPosition(10);
    }

    public void Init(Vector3 position, int index, Hexagon hex)
    {
        transform.localPosition = position;
        this.hex = hex;
        this.index = index;
    }

    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody>();
        controller = GetComponent<CharacterController>();
        currentHealth = enemy.MaxHealth;
        timeOffset = Random.Range(0, 15);
        timeSinceAttack = enemy.timeBetweenAttacks;
        rays = new Ray[3]
        {
            new Ray(transform.position, transform.forward - transform.right),
            new Ray(transform.position, transform.forward),
            new Ray(transform.position, transform.forward + transform.right)
        };
    }

    // Update is called once per frame
    void Update()
    {
        //DirectionChange();
        velocity.y += -50 * Time.deltaTime;
        switch (aiState)
        {
            case AIState.Idle:
                if(timeSinceMove <= enemy.timeBetweenMovements + timeOffset)
                {
                    timeSinceMove += Time.deltaTime;
                }else
                {
                    wanted = GetRandomPosition(15);
                    aiState = AIState.Moving;
                }
                break;
            case AIState.Moving:
                wanted.y= transform.position.y;
                Rotate();
                Vector3 change = DirectionChange();
                controller.Move((transform.forward + change).normalized * Time.deltaTime * enemy.speed);
                if(Vector3.Distance(wanted, transform.position) < 1)
                {
                    timeSinceMove = 0;
                    GetNewRandomTime();
                    aiState = AIState.Idle;
                }
                break;
            case AIState.Chasing:

                    break;
            case AIState.Attacking:
                break;
        }
        controller.Move(velocity * Time.deltaTime);
        velocity.y = 0;
    }

    void Rotate()
    {//find the vector pointing from our position to the target
        _direction = (wanted - transform.position).normalized;

        //create the rotation we need to be in to look at the target
        _lookRotation = Quaternion.LookRotation(_direction);

        //rotate us over time according to speed until we are in the required rotation
        transform.rotation = Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * enemy.rotationSpeed);
    }

    void GetNewRandomTime()
    {
        timeOffset = Random.Range(0, 15);
    }

    protected void Setup()
    {
        currentHealth = enemy.MaxHealth;
    }

    /*Vector3 GetRandomPosition(float yOffset = 0)
    {
        Vector3 wantedPosition = new Vector3(hex.transform.position.x + Random.Range(-16f, 16f),
                                       5 + yOffset,
                                       hex.transform.position.z + Random.Range(-19f, 19f));
        RaycastHit hit;
        if(Physics.Raycast(new Vector3(wantedPosition.x, 1000, wantedPosition.z), Vector3.down, out hit, 2000, GameManager.instance.enemyWalkable))
        {
            if(hit.transform.GetComponent<Hexagon>() == hex)
                return wantedPosition;
            else
                return GetRandomPosition(yOffset);
        }
        else
        {
            return GetRandomPosition(yOffset);
        }
    }*/

    Vector3 GetRandomPosition(float radius)
    {
        Random.InitState(System.DateTime.Now.Millisecond);
        Vector2 circle = Random.insideUnitCircle * radius;
        Vector3 randomPosition = new Vector3(hex.transform.position.x + circle.x, hex.transform.position.y + 5, hex.transform.position.z + circle.y);
        //Debug.Log(circle);
        //randomDirection.y = hex.transform.position.y + 5;
        return randomPosition;
    }

    Vector3 DirectionChange()
    {
        Vector3 dir = Vector3.zero;
        //setup rays
        RaycastHit hit;
        for (int i = 0; i < rays.Length; i++)
        {
            rays[i].origin = transform.position;
        }
        rays[0].direction = transform.forward - transform.right;
        rays[1].direction = transform.forward;
        rays[2].direction = transform.forward + transform.right;
        //left
        if (Physics.Raycast(rays[0], out hit, 2.5f, GameManager.instance.enemyIgnore))
        {
            dir = transform.right;
        }//forward
        else if (Physics.Raycast(rays[1], out hit, 2.5f, GameManager.instance.enemyIgnore))
        {
            dir = transform.right;
        }//right
        else if (Physics.Raycast(rays[2], out hit, 2.5f, GameManager.instance.enemyIgnore))
        {
            dir = -transform.right;
        }
        return dir;
    }

    public void DoDmg(float dmgAmount, TOOLTYPE toolType)
    {
        StartCoroutine(GetHit());
        aiState = AIState.Chasing;
        timeSinceMove = 0;
        if (dmgAmount == 0) dmgAmount = 1;
        //dmgAmount *= (toolType == TOOLTYPE.Sword) ? 2 : 1;
        currentHealth -= dmgAmount;
        if (currentHealth <= 0)
        {
            //die
            Die();
        }
    }

    public void Die(bool killed = true)
    {
        if (killed)
        {
            //mesh.enabled = false;
            //destroy enemy
            GameManager.playerInstance.AddExp(enemy.ExpToGive);
            GameManager.playerInstance.inventory.AddItemToInventory(4, enemy.DnaToGive);
        }
        GameManager.enemies.RemoveAt(index);
        hex.enemies.Remove(this);
        Destroy(gameObject);
    }

    IEnumerator GetHit()
    {
        pushback = true;
        Vector3 wantedPos = transform.position + GameManager.playerInstance.transform.forward * 3;
        float dis = Vector3.Distance(transform.position, wantedPos);
        while (dis >= 0.5f)
        {
            transform.position = Vector3.Lerp(transform.position, wantedPos, Time.deltaTime * 5);
            dis = Vector3.Distance(transform.position, wantedPos);
            yield return new WaitForEndOfFrame();
        }
        pushback = false;
        yield break;
    }

    public void Use(float dmg, TOOLTYPE type)
    {
        DoDmg(dmg, type);
    }
}

#ifndef TB_COMPONENT_H
#define TB_COMPONENT_H

class TbComponent {
public:
    // Virtual destructor ensures derived classes are cleaned up properly
    virtual ~TbComponent() = default;

    // Pure virtual methods: any class inheriting this MUST implement these
    virtual void reset() = 0;
    virtual void run() = 0;
};

#endif // TB_COMPONENT_H